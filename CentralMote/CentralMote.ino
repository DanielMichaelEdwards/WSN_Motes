// inslude the SPI library:
#include <SPI.h>
#include <stdint.h>
#include "ADF7030.h"
const uint8_t Radio_Memory_Configuration[ ] = { 
  //#include "C:\Users\95ry9\Documents\Varsity\Fourth_Year\ELEN4002_Lab_Proj\WSN_Motes\ConfigSetup\IEEE_Packet_Format.cfg"
  #include "C:\Users\Daniel\Documents\Daniel\Varsity\4th Year\Lab Project\Arduino Code\WSN_Motes\Hop\IEEE_Packet_Format.cfg" 
  };

ADF7030 adf7030;

const int Size_OF = sizeof(Radio_Memory_Configuration);

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
//const int checking = 12;

uint8_t minerAddr = 0xFF;
uint8_t routes[3][3] = {{0x00, 0x00, 0x01},{0x01,0x00, 0x02},{0x02,0x01, 0x03}};//First size is the number of nodes. ie: [#nodes][#addresses]
int receivedVal = 0;
const int TX_PKT_BTN = 2;
const int RX_LED = 4;
const int TX_LED = 5;
const int ALRT_LED = 6;
void setup() {
  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);
  pinMode(7,OUTPUT);
  digitalWrite(slaveSelectPin, HIGH);
  Serial.begin(2400);
  // initialize SPI:
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  pinMode(TX_PKT_BTN, INPUT);

  //Set Transceiver as SLave active
  digitalWrite(slaveSelectPin, LOW);
  //Run the configuration
  SPI.transfer(0b10000101); 
  //Disbale Transceiver as Slave
  digitalWrite(slaveSelectPin, HIGH);
}

int Data = 0;
void loop() {
  /*String cmd;
  if (Serial.available() > 0)
  {
    cmd = Serial.readString();   

  if (cmd == "RSSI")
  {
    Serial.println("h");
  }
    //cmd += c;  
    
  }*/
  

    Serial.print("ADF7030 is configuring...");
///////////////////////////////////////////////////
  //Start of Powerup From Cold sequence
//////////////////////////////////////////////////
  //adf7030.Power_Up_From_Cold();
////////////////////////////////////////////
  //End Of Power start up sequence
/////////////////////////////////////////////


///////////////////////////////////////////
//Start of Config Sequence
/////////////////////////////////////////
  
  
  int temp = 0;
  long Value = 0;
  int count = 0;
  int y =0;
  
  while (count < Size_OF)
  {
    digitalWrite(slaveSelectPin, LOW);
    temp = count +3;
    y = 2;
  for(int i = count; i<temp;i++)
  {
    long power = pow(256,y)+0.5;
    Value += power*Radio_Memory_Configuration[i];
    y--;
  }
  count = temp;

  for(int i = count; i< Value;i++)
  {
    receivedVal = SPI.transfer(Radio_Memory_Configuration[i]);
  }

  digitalWrite(slaveSelectPin, HIGH);
  count = Value;
  }
  adf7030.Configure_ADF7030();  
  Serial.println("Ready to operate");
  /////////////////////////////////////
  //Central  Mote Code
  ////////////////////////////////////
  adf7030.Power_Up_From_Cold();
  adf7030.Configure_ADF7030();
  adf7030.Go_To_PHY_ON();
  uint8_t Data_PHR[] = {0x47, 0xFC, 0xC0, 0xEE};
  adf7030.Write_To_Register(0x20000510, Data_PHR,1);
  requestRSSI(0xFF);
  while(1)
   {
        digitalWrite(RX_LED, HIGH);
        Serial.print("Start Receiving\n\n");    
        adf7030.Receive(0x20000C18,2);    
        Serial.print("Finish Receiving\n\n");
        adf7030.Read_Register(0x20000C18, 2);
        uint8_t receivedData [8];
        adf7030.Get_Register_Data(0x20000C18,2, receivedData); 
        if (receivedData[3] == 0x00)
        {
          digitalWrite(RX_LED, LOW);
          if (receivedData[4] == 0x0A)
          {
            digitalWrite(ALRT_LED, HIGH);
            //A miner is in distress
            Serial.println("\n\nALERT! ALERT! Miner distress received!!");
            uint8_t sector = receivedData[0];
            if (sector == 0xFF)
            {
              //The message is straight from a miner
              Serial.println("Miner in unknown sector.");
            }
            else
            {
              //The first hop is the sector of the miner
              Serial.print("Miner in sector: ");
              Serial.println(sector, DEC);
            }
            uint8_t minerStatusByte = receivedData[5];
            decodeMinerInfo(minerStatusByte);
            sendMessageReceived();
          }
        }
    }
}

void sendCMD()
{
  Serial.println("Send cmd");
}

void sendCommand()
{
  int rssiIO = analogRead(5);
  int resendIO = analogRead(4);
  int msgReceivedIO = analogRead(3);
  if (rssiIO > 1000)
  {
    requestRSSI(0xFF);
  }
  if (resendIO > 1000)
  {
    requestResendMinerStatus();
  }
  if (msgReceivedIO > 1000)
  {
    sendMessageReceived();
  }
}

void requestRSSI(uint8_t Addr)
{
  Serial.println("\n\n Request mote RSSI value...");
  uint8_t Data[] = {0x00, 0x00, 0x01, Addr, 0x81, 0x00, 0x00, 0x00};
  adf7030.Write_To_Register(0x20000AF0,Data,2);
  if (adf7030.Transmit() == false)
  {
    return;
  }
  Serial.print("Start Receiving\n\n");  
  uint8_t receivedData [] = {0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
  int timer = millis();
  int elapsedTime = 0;
  /*while(receivedData[3] != 0x00)
  {*/
    adf7030.Wait_For_Reply(0x20000C18,2);
    /*elapsedTime = millis() - timer;
    Serial.println(elapsedTime);
    if (elapsedTime > 30000)
    {
      Serial.println("RSSI not received.");
      return;
    }
  }
  Serial.print("Finish Receiving\n\n");*/
  
  
  //adf7030.Read_Register(0x20000C18, 2);
  adf7030.Get_Register_Data(0x20000C18,2, receivedData);
  uint8_t Data_RSSI [2];
  Data_RSSI[0] = receivedData[5];
  Data_RSSI[1] = receivedData[6];
  uint16_t rssiBin = Data_RSSI[1];
  rssiBin = rssiBin + (Data_RSSI[0] << 8);
  uint16_t rssi2s = ~rssiBin;
  rssi2s = rssi2s + (0b01);

  int num = rssiBin >> 10;
  
  float val =  rssi2s - 63488;
  val *= 0.25;

  if (num == 1)
  {
    val *= -1;
  }
  Serial.print("\n\n RSSI value: ");
  Serial.print(val);
  Serial.println("dBm");
}

void requestTemp(uint8_t Addr)
{
  Serial.println("\n\n Request mote temperature value...");
  uint8_t Data[] = {0x00, 0x00, 0x01, Addr, 0x80, 0x00, 0x00, 0x00};
  adf7030.Write_To_Register(0x20000AF0,Data,2);
  if (adf7030.Transmit() == false)
  {
    return;
  }
  Serial.print("Start Receiving\n\n");    
  if (adf7030.Wait_For_Reply(0x20000C18,2) == false)
  {
    return;
  }
  Serial.print("Finish Receiving\n\n");
  uint8_t receivedData [8];
  adf7030.Get_Register_Data(0x20000C18,2, receivedData);
  uint16_t tempBin = receivedData[5];
  tempBin = tempBin + (receivedData[6] << 8);
  tempBin = tempBin + (receivedData[7] << 16);

  float temp = tempBin;
  if (temp > 2048)
  {
    temp = temp - 4096;
  }
  temp *= 0.0625;

  Serial.print("\n\n Temperature value: ");
  Serial.println(temp);
}

void requestResendMinerStatus()
{
  //Always from a miner
  Serial.println("\n\n Request miner status value...");
  uint8_t Data[] = {0x00, 0x00, 0x01, 0xFF, 0x82, 0x00, 0x00, 0x00};
  adf7030.Write_To_Register(0x20000AF0,Data,2);
  if (adf7030.Transmit() == false)
  {
    return;
  }
  Serial.print("Start Receiving\n\n");    
  if (adf7030.Wait_For_Reply(0x20000C18,2) == false)
  {
    return;
  }
  Serial.print("Finish Receiving\n\n");
  uint8_t receivedData [8];
  adf7030.Get_Register_Data(0x20000C18,2, receivedData);
  uint8_t minerStatusByte = receivedData[5];
  uint8_t alertByte = receivedData[4];
  if (alertByte == 0x0A)
  {
    Serial.println("\n\nALERT! ALERT! Miner distress received!!");
  }
  else {
    Serial.println("\n\n Miner status received.");
    //Update LED
  }
  decodeMinerInfo(minerStatusByte);
  sendMessageReceived();
}

void sendMessageReceived()
{
  Serial.println("\n\n Inform Miner that message has been received...");
  uint8_t Data[] = {0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00};
  adf7030.Write_To_Register(0x20000AF0,Data,2);
  if (adf7030.Transmit() == false)
  {
    return;
  }
}

void notifyHelpOnTheWay()
{
  Serial.println("\n\nInform Miner that help is on the way...");
  uint8_t Data[] = {0x00, 0x00, 0x01, 0xFF, 0x01, 0x00, 0x00, 0x00};
  adf7030.Write_To_Register(0x20000AF0,Data,2);
  if (adf7030.Transmit() == false)
  {
    return;
  }
  digitalWrite(ALRT_LED, LOW);
}

void decodeMinerInfo(uint8_t minerStatusByte)
{
  boolean injuredIO = (minerStatusByte & 0x01) != 0;
  boolean environIO = (minerStatusByte & 0x02) != 0;
  boolean trappedIO = (minerStatusByte & 0x04) != 0;
  if (injuredIO)
  {
    Serial.println("Miner injured.");
    //Update LED
  }
  if (environIO)
  {
    Serial.println("Environmental dangers present.");
    //Update LED
  }
  if (trappedIO)
  {
    Serial.println("Miners are trapped!");
    //Update LED
  }  
}

