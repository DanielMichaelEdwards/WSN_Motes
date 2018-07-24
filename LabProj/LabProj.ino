// inslude the SPI library:
#include <SPI.h>
#include <stdint.h>

// set pin 10 as the slave select for the Transceiver:
const int slaveSelectPin = 10;

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


  //Set Transceiver as SLave active
  digitalWrite(slaveSelectPin, LOW);
  //Run the configuration
  SPI.transfer(0b10000101); 
  //Disbale Transceiver as Slave
  digitalWrite(slaveSelectPin, HIGH);
}


void loop() {

Read_Register(0x400042B4,2);
  
while(1){}
}
