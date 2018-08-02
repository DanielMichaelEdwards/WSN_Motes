#include "ADF7030.h"
#include "Arduino.h"
#include "SPI.h"

ADF7030::ADF7030() {  
}

void ADF7030::Read_Register(uint32_t Address, int Iterations){

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

void ADF7030::Poll_Status_Byte ()
 {
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
}

void ADF7030::Power_Up_From_Cold() {

  
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

  Poll_Status_Byte();

  Serial.print(CMD_Ready);
  Serial.print("\t");
  Serial.print(Idle_State_2);
  Serial.println(Idle_State_1);

  
  digitalWrite(slaveSelectPin, HIGH);
  
}

void ADF7030::Wait_For_CMD_Ready ()
{
  receivedVal = SPI.transfer(0xFF);
  CMD_Ready = bitRead(receivedVal,5);

  while(CMD_Ready == 0)
  {
    receivedVal = SPI.transfer(0xFF);
    CMD_Ready = bitRead(receivedVal,5);
  }
  
}

void ADF7030::Configure_ADF7030 () {
  digitalWrite(slaveSelectPin, LOW);
  receivedVal = SPI.transfer(0b10000101);
  Serial.println(receivedVal,HEX);
  digitalWrite(slaveSelectPin, HIGH);

  digitalWrite(slaveSelectPin, LOW);

  Wait_For_CMD_Ready();

  Poll_Status_Byte();

  Serial.print(CMD_Ready);
  Serial.print("\t");
  Serial.print(Idle_State_2);
  Serial.println(Idle_State_1);
  digitalWrite(slaveSelectPin, HIGH);

  Read_Register(0x20000514,2);
}
