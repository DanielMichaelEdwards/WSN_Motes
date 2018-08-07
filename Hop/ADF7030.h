#ifndef ADF7030_h
#define ADF7030_h

#include "Arduino.h"
#include "SPI.h"

class ADF7030 {
  public:
    ADF7030();
    void Read_Register(uint32_t Address, int Iterations);
    void Write_To_Register(uint32_t Address, uint8_t Data[]);
    void Poll_Status_Byte(int bit2, int bit1);
    void Power_Up_From_Cold();
    void Wait_For_CMD_Ready ();
    void Configure_ADF7030 ();
    void Read_MISC_FW();
    void Go_To_PHY_ON();
    void Go_To_PHY_OFF();
    void Transmit();
    void Receive(uint32_t Address, int Iterations);
    int receivedVal=0;
    int CMD_Ready = 0;
    int Idle_State_1 = 0;
    int Idle_State_2 = 0;
    const int slaveSelectPin = 10;
};

#endif

