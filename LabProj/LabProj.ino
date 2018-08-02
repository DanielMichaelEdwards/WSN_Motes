// inslude the SPI library:
#include <SPI.h>
#include <stdint.h>
#include "ADF7030.h"

// set pin 10 as the slave select for the Transceiver:
const int slaveSelectPin = 10;

ADF7030 adf7030;

 
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

int Data = 0;
void loop() {

  //adf7030.Configure_ADF7030();
/*
 
  digitalWrite(slaveSelectPin, LOW);
  Data = SPI.transfer(0b00111000);
  Serial.println(Data,HEX);

  Data = SPI.transfer(0x20);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0x00);
  Serial.println(Data,HEX);
  Data = SPI.transfer(0x0A);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0xF0);
  Serial.println(Data,HEX);

  Data = SPI.transfer(0x01);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0xBE);
  Serial.println(Data,HEX);
  Data = SPI.transfer(0xCD);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0xDE);
  Serial.println(Data,HEX);
  Data = SPI.transfer(0x02);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0xBD);
  Serial.println(Data,HEX);
  Data = SPI.transfer(0xF0);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0xFF);
  Serial.println(Data,HEX);
  
  digitalWrite(slaveSelectPin, HIGH);
  */


  adf7030.Read_Register(0x20000AF0,1);

  delay(100);

  digitalWrite(slaveSelectPin, LOW);
  SPI.transfer(0x84);
  digitalWrite(slaveSelectPin, HIGH);
  
  

  //adf7030.Read_Register(0x400042B4,1);

  adf7030.Read_Register(0x20000AF0,1);
  
  adf7030.Power_Up_From_Cold();
  adf7030.Configure_ADF7030();
  adf7030.Go_To_PHY_ON();
  adf7030.Transmit();

  adf7030.Read_Register(0x20000514,1);

while(1){
  
  
  //Read_Register(0x400042B4,1);
  //delay(1000);}
}
}
