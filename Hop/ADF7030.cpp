#include "ADF7030.h"
#include "Arduino.h"
#include "SPI.h"

ADF7030::ADF7030() {  
}

void ADF7030::Read_Register(uint32_t Address, int Iterations){

  Serial.println("\n\n Reading register...");
  uint8_t AddressArray[4];
  uint8_t ReceivedData = 0;
  AddressArray[0] = Address >> 24;
  AddressArray[1] = Address >> 16;
  AddressArray[2] = Address >>  8;
  AddressArray[3] = Address;

  digitalWrite(slaveSelectPin, LOW);
  
  ReceivedData = SPI.transfer(0b01111000);
  Serial.println("Registers data");
  for(int i=0;i<4;i++)
  {
    ReceivedData = SPI.transfer(AddressArray[i]);
  }

  ReceivedData = SPI.transfer(0xFF);
  ReceivedData = SPI.transfer(0xFF);
  
  for(int j=0; j< Iterations*4;j++)
  {
    ReceivedData = SPI.transfer(0xFF);
    Serial.println(ReceivedData,HEX);
  }
  
  digitalWrite(slaveSelectPin,HIGH);
}

void ADF7030::Get_Register_Data(uint32_t Address, int Iterations, uint8_t RegisterData[])
{
  Serial.println("\n\n Fetching register data...");
  uint8_t AddressArray[4];
  uint8_t ReceivedData = 0;
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
  
  for(int j=0; j< Iterations*4;j++)
  {
    RegisterData[j] = SPI.transfer(0xFF);
  }
  
  digitalWrite(slaveSelectPin,HIGH);
}

void ADF7030::Read_Received(int Iterations, uint8_t RegisterData[]) 
{
  
  Serial.println("\n\n Reading received register...");
  uint8_t AddressArray[] = {0x20, 0x00, 0x0C, 0x18};
  uint8_t ReceivedData = 0;

  digitalWrite(slaveSelectPin, LOW);
  
  ReceivedData = SPI.transfer(0b01111000);
  for(int i=0;i<4;i++)
  {
    ReceivedData = SPI.transfer(AddressArray[i]);
  }

  ReceivedData = SPI.transfer(0xFF);
  ReceivedData = SPI.transfer(0xFF);
  
  for(int j=0; j< Iterations*4;j++)
  {
    RegisterData[j] = SPI.transfer(0xFF);
  }
  
  digitalWrite(slaveSelectPin,HIGH);
}

void ADF7030::Write_To_Register(uint32_t Address, uint8_t Data[], int dataSize)
{
  Serial.println("\n\n Writing to register...");
  uint8_t AddressArray[4];
  int ReceivedData = 0;
  AddressArray[0] = Address >> 24;
  AddressArray[1] = Address >> 16;
  AddressArray[2] = Address >>  8;
  AddressArray[3] = Address;

  digitalWrite(slaveSelectPin, LOW);
  
  ReceivedData = SPI.transfer(0b00111000);

  for(int i=0;i<4;i++)
  {
    ReceivedData = SPI.transfer(AddressArray[i]);
  }
  
  Serial.println("\n\n Register Data:");
  
  for(int j=0; j< dataSize*4;j++)
  {
    ReceivedData = SPI.transfer(Data[j]);
    Serial.println(ReceivedData, HEX);
  }
  
  digitalWrite(slaveSelectPin,HIGH);
  
}

void ADF7030::Write_Register_Short(uint8_t Pointer, uint8_t Offset, uint8_t Data[], int dataSize)
{
  Serial.println("\n\n Writing to register (short) ...");
  uint8_t cmd = 0b00110100;  
  cmd += Pointer;
  int ReceivedData = 0;
  digitalWrite(slaveSelectPin, LOW);
  ReceivedData = SPI.transfer(cmd);
  ReceivedData = SPI.transfer(Offset);
  for(int j=0; j<dataSize;j++)
  {
    ReceivedData = SPI.transfer(Data[j]);
  } 
  digitalWrite(slaveSelectPin, HIGH);  
}

float ADF7030::Get_RSSI()
{
  Serial.println("\n\n Getting RSSI value...");
  uint8_t Data_RSSI [4];
  Get_Register_Data(0x20000538,1, Data_RSSI);
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
  return val;
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

  //Read_Register(0x20000514,2);
}

void ADF7030::Go_To_PHY_ON()
{
  Serial.println("\n\n Go to PHY_ON...");
  digitalWrite(slaveSelectPin, LOW);
  receivedVal = SPI.transfer(0x82);  
  Wait_For_CMD_Ready();
  Poll_Status_Byte(1,0);
  digitalWrite(slaveSelectPin, HIGH);
  //Read_Register(0x400042B4,1);
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
  //Read_Register(Address, Iterations); 

}

void ADF7030::Read_MISC_FW() {
  //Read_Register(0x400042B4,1);
}

