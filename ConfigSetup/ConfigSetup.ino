// inslude the SPI library:
#include <SPI.h>
#include "ADF7030.h"
const uint8_t Radio_Memory_Configuration[ ] = { 
  #include "C:\Users\Daniel\Documents\Daniel\Varsity\4th Year\Lab Project\Arduino Code\WSN_Motes\ConfigSetup\New.cfg" 
  };

ADF7030 adf7030;

const int Size_OF = sizeof(Radio_Memory_Configuration);

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
//const int checking = 12;

int x =0;

int receivedVal = 0;

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
  adf7030.Power_Up_From_Cold();
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
    Serial.println(Value);
    y--;
  }
  count = temp;

  for(int i = count; i< Value;i++)
  {
    receivedVal = SPI.transfer(Radio_Memory_Configuration[i]);
    Serial.print(receivedVal,HEX);
    Serial.print("\t");
    Serial.println(Radio_Memory_Configuration[i],HEX);
  }

  digitalWrite(slaveSelectPin, HIGH);
  count = Value;
  Serial.print("Count \t");
  Serial.println(count);
  Serial.println(Value,HEX);
  //count = Size_OF;
  }
  adf7030.Configure_ADF7030();
  x++;
  }
}
