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

uint8_t myAddrIndex = 1;
uint8_t minerAddr = 0xFF;
uint8_t routes[3][3] = {{0x00, 0x00, 0x01},{0x01,0x00, 0x02},{0x02,0x01, 0x03}};//First size is the number of nodes. ie: [#nodes][#addresses]
int receivedVal = 0;

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
  //Hop Things
  ////////////////////////////////////
  uint8_t Data[] = {0x20, 0x00, 0x0A, 0xF0};
  adf7030.Write_To_Register(0x40001800,Data,1);
  uint8_t Data_PHR[] = {0x47, 0xFC, 0xC0, 0xEE};
  adf7030.Write_To_Register(0x20000510, Data_PHR,1);
  uint8_t Header_Data[] = {0x03, 0x02, 0x01,0x00};
  adf7030.Write_Register_Short(0b01, 0x00, Header_Data, 4);
  uint8_t registerData[4];
  adf7030.Read_Received(1, registerData);
  adf7030.Go_To_PHY_ON();
  Serial.println(registerData[0], HEX);

  while(1)
  {
    Serial.print("Start Receiving\n\n");    
    adf7030.Receive(0x20000C18,1);    
    Serial.print("Finish Receiving\n\n");
    adf7030.Read_Register(0x20000AF0,1);
    uint8_t registerData[4];
    adf7030.Read_Received(1, registerData);
    if ((registerData[0] == registerData[1]) && (registerData[0] == minerAddr))
    {
      //This packet is from a miners mote.
      //Set the source to be the address of this mote.
      uint8_t updatedSource[] = {routes[myAddrIndex][0], registerData[1], registerData[2], registerData[3]};
      forwardPacket(updatedSource);      
    }
    
    if (registerData[2] == routes[myAddrIndex][0])
    {
      //I need to do something with this information
      if (registerData[3] == routes[myAddrIndex][0])
      {
        //This data is for me!
        digitalWrite(7, HIGH);
        delay(1000);
        digitalWrite(7, LOW);
      }
      else 
      {     
        //I was meant to receive this, but I must forward it.
        forwardPacket(registerData);
      }
    }
    else {
      //I received it but don't do anything with it.
      digitalWrite(6,HIGH);
      delay(1000);
      digitalWrite(6, LOW);
    }
  }  
}  

void forwardPacket(uint8_t registerData[])
{
  if (registerData[3] == routes[0][0])
  {
    //This data is going to the central node (forward direction).
    digitalWrite(8, HIGH);
    adf7030.Write_To_Register(0x20000AF0, registerData,1);
    uint8_t addrInfo[4] = {registerData[3],routes[myAddrIndex][1],routes[myAddrIndex][0], registerData[0]};
    adf7030.Write_Register_Short(0b01, 0x00, addrInfo, 4);
    adf7030.Read_Register(0x20000AF0,2);
    adf7030.Transmit();
    digitalWrite(8, LOW);
  }
  else 
  {
    //This data is going away from the central node (backward direction).
    digitalWrite(8, HIGH);
    adf7030.Write_To_Register(0x20000AF0, registerData,1);
    uint8_t addrInfo[4] = {registerData[3],routes[myAddrIndex][2],routes[myAddrIndex][0], registerData[0]};
    adf7030.Write_Register_Short(0b01, 0x00, addrInfo, 4);
    adf7030.Read_Register(0x20000AF0,2);
    adf7030.Transmit();
    digitalWrite(8, LOW);
  }    
}

