#ifndef _DEV_CONFIG_SERVO_H_
#define _DEV_CONFIG_SERVO_H_

#include <stdint.h>
#include <stdio.h>
#include <termio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

#define IIC_Write(_add, _dat) IIC_Write(_add, _dat)
#define IIC_Read(_add) IC_Read(_add)
#define IIC_Addr 0x40
#define DEV_Delay_ms(__xms) delay(__xms)

class DEV_Servo_Config
{
 public:
  DEV_Servo_Config();

  void IIC_Write(UBYTE add_, UBYTE data_);
  UBYTE IC_Read(UBYTE add_);

  UBYTE DEV_ModuleInit(void);
  void DEV_ModuleExit(void);
};
#endif
