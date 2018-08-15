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
  pinMode(7,OUTPUT);
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

 
  /*digitalWrite(slaveSelectPin, LOW);
  Data = SPI.transfer(0b00111000);
  Serial.println(Data,HEX);

  Data = SPI.transfer(0x20);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0x00);
  Serial.println(Data,HEX);
  Data = SPI.transfer(0x05);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0x10);
  Serial.println(Data,HEX);
  
  Data = SPI.transfer(0x47);
  Data = SPI.transfer(0xFC);
  Data = SPI.transfer(0xC0);
  Data = SPI.transfer(0x00);

  digitalWrite(slaveSelectPin, HIGH);
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
  
  Data = SPI.transfer(0xAA);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0xBB);
  Serial.println(Data,HEX);
  Data = SPI.transfer(0xCC);
  Serial.println(Data,HEX); 
  Data = SPI.transfer(0xDD);
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



  /*adf7030.Read_Register(0x20000AF0,1);

  delay(100);

  digitalWrite(slaveSelectPin, LOW);
  SPI.transfer(0x84);
  digitalWrite(slaveSelectPin, HIGH);
  */
  

  /*adf7030.Power_Up_From_Cold();
  adf7030.Configure_ADF7030();
  adf7030.Go_To_PHY_ON();
  adf7030.Read_Register(0x20000500,1);
  adf7030.Read_Register(0x20000514,2);
  //digitalWrite(7, HIGH);
  adf7030.Receive(0x20000C18,2);
  //digitalWrite(7, LOW);
  //digitalWrite(7, HIGH);
  adf7030.Receive(0x20000C18,2);
  //digitalWrite(7, LOW);
  Serial.print("Finish Receiving\n\n");
  adf7030.Read_Register(0x20000C18,2);*/

  /*adf7030.Read_Register(0x20000AF0,1);
  */
  adf7030.Power_Up_From_Cold();
  adf7030.Configure_ADF7030();
  adf7030.Read_Register(0x20000380,1);
  adf7030.Go_To_PHY_ON();
  uint8_t Data_PHR[] = {0x47, 0xFC, 0xC0, 0xEE};
  adf7030.Write_To_Register(0x20000510, Data_PHR,1);
  uint8_t Data[] = {0x00, 0x00, 0x01, 0xFF, 0x80, 0x01, 0x00, 0x00};
  adf7030.Write_To_Register(0x20000AF0,Data,2);
  adf7030.Read_Register(0x40001800,1);
  adf7030.Transmit();
  Serial.print("Start Receiving\n\n");    
  adf7030.Receive(0x20000C18,1);    
  Serial.print("Finish Receiving\n\n");
  adf7030.Read_Register(0x20000C18,1);
  float num = adf7030.Get_RSSI();
  Serial.println(num);
  //adf7030.Read_Register(0x400042B4,1);

while(1)
  {
    //Read_Register(0x400042B4,1);
    //delay(1000);}
  }
}
