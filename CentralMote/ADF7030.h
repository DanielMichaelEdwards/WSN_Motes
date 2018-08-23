#ifndef ADF7030_h
#define ADF7030_h

#include "Arduino.h"
#include "SPI.h"

class ADF7030 {
  public:
    ADF7030();
    void Read_Register(uint32_t Address, int Iterations);
    void Get_Register_Data(uint32_t Address, int Iterations, uint8_t RegisterData[]);
    float Get_RSSI();
    void Read_Received(int Iterations, uint8_t RegisterData[]);
    void Write_To_Register(uint32_t Address, uint8_t Data[], int dataSize);
    void Write_Register_Short(uint8_t Pointer, uint8_t Offset, uint8_t Data[], int dataSize);
    bool Poll_Status_Byte(int bit2, int bit1, bool timeOutFlag);
    void Power_Up_From_Cold();
    void Wait_For_CMD_Ready ();
    void Configure_ADF7030 ();
    void Read_MISC_FW();
    void Go_To_PHY_ON();
    void Go_To_PHY_OFF();
    bool Transmit();
    void Receive(uint32_t Address, int Iterations);
    bool Wait_For_Reply(uint32_t Address, int Iterations);
    int receivedVal=0;
    int CMD_Ready = 0;
    int Idle_State_1 = 0;
    int Idle_State_2 = 0;
    const int slaveSelectPin = 10;
};

#endif

