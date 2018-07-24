// inslude the SPI library:
#include <SPI.h>

const uint8_t Radio_Memory_Configuration[ ] = { 
  #include "C:\Users\95ry9\Documents\Arduino\ConfigSetup\New.cfg" 
  };


const int Size_OF = sizeof(Radio_Memory_Configuration);

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
//const int checking = 12;

int x =0;

int receivedVal = 0;

int CMD_Ready = 0;

int Idle_State_1 = 0;
int Idle_State_2 = 0;

void Read_Register(uint32_t Address, int Iterations){

  uint8_t AddressArray[4];
  int ReceivedData = 0;
  AddressArray[0] = Address >> 24;
  AddressArray[1] = Address >> 16;
  AddressArray[2] = Address >>  8;
  AddressArray[3] = Address;

  digitalWrite(slaveSelectPin, LOW);

  Serial.println("Status:");
  
  ReceivedData = SPI.transfer(0b01111000);
  Serial.println(ReceivedData,HEX);

  for(int i=0;i<4;i++)
  {
    ReceivedData = SPI.transfer(AddressArray[i]);
    Serial.println(ReceivedData,HEX);
  }

  ReceivedData = SPI.transfer(0xFF);
  Serial.println(ReceivedData,HEX);
  ReceivedData = SPI.transfer(0xFF);
  Serial.println(ReceivedData,HEX);

  Serial.println("");
  Serial.println("Register Data:");
  
  for(int j=0; j< Iterations*4;j++)
  {
    ReceivedData = SPI.transfer(0xFF);
    Serial.println(ReceivedData,HEX);
  }
  
  digitalWrite(slaveSelectPin,HIGH);
 }

void setup() {
  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);
  digitalWrite(slaveSelectPin, HIGH);
  Serial.begin(2400);
  // initialize SPI:
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
}


void loop() {
  if(x == 0){
///////////////////////////////////////////////////
  //Start of Powerup From Cold sequence
//////////////////////////////////////////////////
  digitalWrite(slaveSelectPin, LOW);

  receivedVal = SPI.transfer(0xFF);
  //Serial.println(receivedVal,HEX);

  while (digitalRead(12) == 0)
  {
    receivedVal = SPI.transfer(0xFF);
    Serial.println("MISO is Low");
  }

  digitalWrite(slaveSelectPin, HIGH);
  digitalWrite(slaveSelectPin, LOW);

  receivedVal = SPI.transfer(0xFF);

  CMD_Ready = bitRead(receivedVal,5);
  Idle_State_1 = bitRead(receivedVal,1);
  Idle_State_2 = bitRead(receivedVal,2);

  while(CMD_Ready == 0 || Idle_State_2 ==0 || Idle_State_1 == 1)
  {
    receivedVal = SPI.transfer(0xFF);
    CMD_Ready = bitRead(receivedVal,5);
    Idle_State_1 = bitRead(receivedVal,1);
    Idle_State_2 = bitRead(receivedVal,2);
  }

  Serial.print(CMD_Ready);
  Serial.print("\t");
  Serial.print(Idle_State_2);
  Serial.println(Idle_State_1);

  
  digitalWrite(slaveSelectPin, HIGH);
////////////////////////////////////////////
  //End Of Power starteup sequence
/////////////////////////////////////////////


///////////////////////////////////////////
//Start of Config Sequence
/////////////////////////////////////////
  
  digitalWrite(slaveSelectPin, LOW);
    
  for(int i = 276; i<304;i++)
  {
    receivedVal = SPI.transfer(Radio_Memory_Configuration[i]);
    Serial.print(receivedVal,HEX);
    Serial.print("\t");
    Serial.println(Radio_Memory_Configuration[i],HEX);
  }
  
  receivedVal = SPI.transfer(0b10000101);
  Serial.println(receivedVal,HEX);
  digitalWrite(slaveSelectPin, HIGH);

  digitalWrite(slaveSelectPin, LOW);

  receivedVal = SPI.transfer(0xFF);

  CMD_Ready = bitRead(receivedVal,5);

  while(CMD_Ready == 0)
  {
    receivedVal = SPI.transfer(0xFF);
    CMD_Ready = bitRead(receivedVal,5);
  }

  receivedVal = SPI.transfer(0xFF);

  CMD_Ready = bitRead(receivedVal,5);
  Idle_State_1 = bitRead(receivedVal,1);
  Idle_State_2 = bitRead(receivedVal,2);

  while(CMD_Ready == 0 || Idle_State_2 ==0 || Idle_State_1 == 1)
  {
    receivedVal = SPI.transfer(0xFF);
    CMD_Ready = bitRead(receivedVal,5);
    Idle_State_1 = bitRead(receivedVal,1);
    Idle_State_2 = bitRead(receivedVal,2);
  }

  Serial.print(CMD_Ready);
  Serial.print("\t");
  Serial.print(Idle_State_2);
  Serial.println(Idle_State_1);
  x++;
  digitalWrite(slaveSelectPin, HIGH);

  Read_Register(0x200002E4,2);
  }
}
