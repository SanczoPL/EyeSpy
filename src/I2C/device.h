#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include "../tools.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

class Device
{
 public:
  Device();

  int Device_ModuleInit(void);
  void Device_ModuleExit(void);

  int Motor_I2C_Init(char addr);
  int Servo_I2C_Init(char addr);
  void Motor_I2C_WriteByte(UBYTE reg, UBYTE value);
  void Servo_I2C_WriteByte(UBYTE reg, UBYTE value);
  UBYTE Motor_I2C_ReadByte(UBYTE reg);
  UBYTE Servo_I2C_ReadByte(UBYTE reg);

  void Device_Delay_ms(UDOUBLE xms);
  void Device_Delay_us(UDOUBLE xus);

 private:
  int m_motor;
  int m_servo;
};

#endif /* DEVICE_H */
