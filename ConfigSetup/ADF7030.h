#ifndef ADF7030_h
#define ADF7030_h

#include "Arduino.h"
#include "SPI.h"

class ADF7030 {
  public:
    ADF7030();
    void Read_Register(uint32_t Address, int Iterations);
    void Poll_Status_Byte();
    void Power_Up_From_Cold();
    void Wait_For_CMD_Ready ();
    void Configure_ADF7030 ();
    int receivedVal=0;
    int CMD_Ready = 0;
    int Idle_State_1 = 0;
    int Idle_State_2 = 0;
    const int slaveSelectPin = 10;
};

#endif

