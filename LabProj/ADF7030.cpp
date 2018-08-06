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
  
  ReceivedData = SPI.transfer(0b01111000);

  for(int i=0;i<4;i++)
  {
    ReceivedData = SPI.transfer(AddressArray[i]);
  }

  ReceivedData = SPI.transfer(0xFF);
  ReceivedData = SPI.transfer(0xFF);
  
  Serial.println("\n\n Register Data:");
  
  for(int j=0; j< Iterations*4;j++)
  {
    ReceivedData = SPI.transfer(0xFF);
    Serial.println(ReceivedData,HEX);
  }
  
  digitalWrite(slaveSelectPin,HIGH);
 }

void ADF7030::Poll_Status_Byte (int bit2, int bit1)
 {
    Serial.println("\n\n Start Polling");
    receivedVal = SPI.transfer(0xFF);

    Idle_State_1 = bitRead(receivedVal,1);
    Idle_State_2 = bitRead(receivedVal,2);
  
    while( Idle_State_2 !=bit2 || Idle_State_1 != bit1)
    {
      receivedVal = SPI.transfer(0xFF);
      Idle_State_1 = bitRead(receivedVal,1);
      Idle_State_2 = bitRead(receivedVal,2);
    }
}

void ADF7030::Power_Up_From_Cold() {
  Serial.println("\n\n Start power up sequence...");
  digitalWrite(slaveSelectPin, LOW);

  receivedVal = SPI.transfer(0xFF);

  while (digitalRead(12) == 0)
  {
    receivedVal = SPI.transfer(0xFF);
    Serial.println("MISO is Low");
  }

  digitalWrite(slaveSelectPin, HIGH);
  digitalWrite(slaveSelectPin, LOW);
  Poll_Status_Byte(1,0);
 
  digitalWrite(slaveSelectPin, HIGH);
}

void ADF7030::Wait_For_CMD_Ready ()
{  
  Serial.println("\n\n Waiting for CMD_READY...");
  receivedVal = SPI.transfer(0xFF);
  CMD_Ready = bitRead(receivedVal,5);

  while(CMD_Ready == 0)
  {
    receivedVal = SPI.transfer(0xFF);
    CMD_Ready = bitRead(receivedVal,5);
  }  
}

void ADF7030::Configure_ADF7030 () {
  Serial.println("\n\n Configuring ADF7030...");
  digitalWrite(slaveSelectPin, LOW);
  receivedVal = SPI.transfer(0b10000101);
  digitalWrite(slaveSelectPin, HIGH);

  digitalWrite(slaveSelectPin, LOW);

  Wait_For_CMD_Ready();

  Poll_Status_Byte(1,0);
  digitalWrite(slaveSelectPin, HIGH);

  Read_Register(0x20000514,2);
}

void ADF7030::Go_To_PHY_ON()
{
  Serial.println("\n\n Go to PHY_ON...");
  digitalWrite(slaveSelectPin, LOW);
  receivedVal = SPI.transfer(0x82);  
  Wait_For_CMD_Ready();
  Poll_Status_Byte(1,0);
  digitalWrite(slaveSelectPin, HIGH);
  Read_Register(0x400042B4,1);
}
void ADF7030::Go_To_PHY_OFF()
{
  Serial.println("\n\n Go to PHY_OFF...");
  digitalWrite(slaveSelectPin, LOW);
  receivedVal = SPI.transfer(0x81);  
  Wait_For_CMD_Ready();
  Poll_Status_Byte(1,0);
  digitalWrite(slaveSelectPin, HIGH);
}


void ADF7030::Transmit() {
  //digitalWrite(8, HIGH);
  Serial.println("\n\n Start Transmission...");
  digitalWrite(slaveSelectPin, LOW);
  receivedVal = SPI.transfer(0x84);//Go to PHY_TX state
  digitalWrite(slaveSelectPin, HIGH);

  digitalWrite(slaveSelectPin, LOW);
  Wait_For_CMD_Ready();
  Poll_Status_Byte(0,1);
  //Need IRQ events
  Poll_Status_Byte(1,0);
  digitalWrite(slaveSelectPin, HIGH);
  Serial.println("Transmission Completed.");
  //digitalWrite(8, LOW);
}

void ADF7030::Receive(uint32_t Address, int Iterations) {
  Serial.print("\n\n Ready to Receive...");
  digitalWrite(slaveSelectPin, LOW);
  receivedVal = SPI.transfer(0x83);//Go to PHY_RX state
  digitalWrite(slaveSelectPin, HIGH);
  digitalWrite(slaveSelectPin, LOW);
  Wait_For_CMD_Ready();  
  Poll_Status_Byte(0,1);
  Serial.print("\n\n Waiting for data...");
  //Need IRQ events
  Poll_Status_Byte(1,0);
  Serial.print("\n\n Data Received.");
  digitalWrite(slaveSelectPin, HIGH);
  Read_Register(Address, Iterations); 

}

void ADF7030::Read_MISC_FW() {
  Read_Register(0x400042B4,1);
}

