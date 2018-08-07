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


int receivedVal = 0;

void setup() {
  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);
  pinMode(7, OUTPUT);
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
  //End Of Power starteup sequence
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
  uint8_t Data[] = {0x20, 0x00, 0x0A, 0xF0};
  adf7030.Write_To_Register(0x40001800,Data);
  adf7030.Read_Register(0x20000AF0, 2);
  adf7030.Read_Register(0x20000C18, 2);
  adf7030.Read_Register(0x40001800, 2);
  uint8_t Data_new[] = {0x86, 0x45, 0x55, 0x66};
  adf7030.Write_Register_Short(0b01, 0x02, Data_new,4); 
  
  adf7030.Read_Register(0x20000AF0, 2);
  adf7030.Read_Register(0x40001800, 2);
  

  /////////////////////////////////////
  //Hop Things
  ////////////////////////////////////

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
  Data = SPI.transfer(0xEE);

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
  Data = SPI.transfer(0xAB);
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
  adf7030.Configure_ADF7030();

  
  Serial.print("ADF7030 configured.");
  adf7030.Go_To_PHY_ON();
  adf7030.Read_Register(0x20000500,1);
  adf7030.Read_Register(0x20000514,2);


  //adf7030.Read_Register(0x20000514,1);
  //adf7030.Read_Register(0x400042B4,1);*/

  while(1)
  {/*
    Serial.print("Start Receiving\n\n");
    digitalWrite(7, HIGH);
    adf7030.Receive(0x20000C18,1);    
    Serial.print("Finish Receiving\n\n");
    adf7030.Read_Register(0x20000AF0,1);
    delay(5000);
    digitalWrite(7, LOW);
    adf7030.Transmit();
    //Read_Register(0x400042B4,1);
    //delay(1000);}*/
  }


  
}
