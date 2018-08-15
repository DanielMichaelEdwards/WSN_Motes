// inslude the SPI library:
#include <SPI.h>
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
const byte sendPin = 2;
void setup() {
  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(slaveSelectPin, HIGH);
  Serial.begin(2400);
  // initialize SPI:
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  attachInterrupt(digitalPinToInterrupt(sendPin), sendMinerStatus, RISING);
  
}

int Data = 0;
void loop() {
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
  //Miner Mote Code
  ////////////////////////////////////
  
  
  uint8_t Data[] = {0x20, 0x00, 0x0A, 0xF0};
  adf7030.Write_To_Register(0x40001800,Data,1);
  uint8_t Data_PHR[] = {0x47, 0xFC, 0xC0, 0xEE};
  adf7030.Write_To_Register(0x20000510, Data_PHR,1);
  uint8_t Header_Data[] = {0x03, 0x02, 0x01,0x00};
  adf7030.Write_Register_Short(0b01, 0x00, Header_Data, 4);
  adf7030.Go_To_PHY_ON();

  while(1)
  {
    Serial.print("Start Receiving\n\n");    
    adf7030.Receive(0x20000C18,1);    
    Serial.print("Finish Receiving\n\n");
    adf7030.Read_Register(0x20000AF0,1);
    uint8_t registerData[8];
    adf7030.Read_Received(2, registerData);
    if (registerData[3] == minerAddr)
    {
      //This packet is intended for a miner mote. Decode it and display the information.
      decodePacket(registerData);
    }
  }  
} 

void sendMinerStatus()
{
  //Take in the input from the switches. 
  //Encode the information
  //Write to the transmit register including header infomation
  //Send the packet
  
}

void decodePacket(uint8_t registerData[])
{
  //Read the receive packet things and output the information to the LED's
  //A 1 in the MSB position of the command field indicates that it is a command, otherwise it is sending information.
  if (registerData[4] == 0x80)
  { 
    //Send temperature
    adf7030.Go_To_PHY_ON();
    //Go to monitoring state
    digitalWrite(slaveSelectPin, LOW);
    receivedVal = SPI.transfer(0x8A);  
    adf7030.Wait_For_CMD_Ready();
    adf7030.Poll_Status_Byte(1,0);
    digitalWrite(slaveSelectPin, HIGH);
    uint8_t Data_TEMP [8];
    adf7030.Get_Register_Data(0X2000038C, 2, Data_TEMP);
    uint8_t Data[] = {minerAddr, minerAddr, 0x01, 0x00, 0x81, Data_TEMP[5], Data_TEMP[6], Data_TEMP[7]};
    adf7030.Write_To_Register(0x20000AF0,Data,2);
    adf7030.Transmit();
    digitalWrite(6, LOW);
  }
  else if (registerData[4] == 0x81)
  { 
    //Send the RSSI
    digitalWrite(6,HIGH);
    uint8_t Data_RSSI [4];
    adf7030.Get_Register_Data(0x20000538,1, Data_RSSI);
    uint8_t Data[] = {minerAddr, minerAddr, 0x01, 0x00, 0x81, Data_RSSI[0], Data_RSSI[1], 0x00};
    adf7030.Write_To_Register(0x20000AF0,Data,2);
    adf7030.Transmit();
    digitalWrite(6, LOW);    
  }
  else if (registerData[4] == 0x82)
  {
    //Resend the status of the miner mote
    sendMinerStatus();
    digitalWrite(7,HIGH);
    delay(1000);
    digitalWrite(7, LOW);
  }
  else if (registerData[4] == 0x00)
  {
    //The central node has received the message that a miner has sent.
  }
  else if (registerData[4] == 0x01)
  {
    digitalWrite(8, HIGH);
    delay(1000);
    digitalWrite(8, LOW);
    //Help is on the way
  }
}

