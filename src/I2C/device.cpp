#include "device.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

Device::Device()
  : m_motor(-1)
  , m_servo(-1)
{
  Device_ModuleInit();
}

int Device::Device_ModuleInit(void)
{
  H_Logger->debug("Device::Device_ModuleInit");

  int ret = wiringPiSetupGpio(); // use BCM2835 Pin number table

  H_Logger->debug("Device::Device_ModuleInit wiringPiSetupGpio ret:{}", ret);
  if (ret < 0)
  {
    H_Logger->error("Device::Device_ModuleInit() wiringPiSetupGpio() failed");
    return -1;
  }
  else
  {
    H_Logger->debug("device::Device_ModuleInit() success");
  }
  return 0;
}

void Device::Device_ModuleExit(void)
{
  // bcm2835_i2c_end();
  // bcm2835_close();
}
int Device::Motor_I2C_Init(char addr)
{
  H_Logger->debug("Device::Motor_I2C_Init addr:{}", addr);
  m_motor = wiringPiI2CSetup(addr);
  return m_motor;
}

int Device::Servo_I2C_Init(char addr)
{
  H_Logger->debug("Device::Servo_I2C_Init addr:{}", addr);
  m_servo = wiringPiI2CSetup(addr);
  return m_servo;
}

void Device::Motor_I2C_WriteByte(UBYTE reg, UBYTE value)
{
  int ref;
  ref = wiringPiI2CWriteReg8(m_motor, (int)reg, (int)value);
  while (ref != 0)
  {
    ref = wiringPiI2CWriteReg8(m_motor, (int)reg, (int)value);
    if (ref == 0) break;
  }
}

void Device::Servo_I2C_WriteByte(UBYTE reg, UBYTE value)
{
  int ref;
  ref = wiringPiI2CWriteReg8(m_servo, (int)reg, (int)value);
  while (ref != 0)
  {
    ref = wiringPiI2CWriteReg8(m_servo, (int)reg, (int)value);
    if (ref == 0) break;
  }
}

UBYTE Device::Motor_I2C_ReadByte(UBYTE reg)
{
  return wiringPiI2CReadReg8(m_motor, reg);
}

UBYTE Device::Servo_I2C_ReadByte(UBYTE reg)
{
  return wiringPiI2CReadReg8(m_servo, reg);
}

void Device::Device_Delay_ms(uint32_t xms)
{
  delay(xms);
}

void Device::Device_Delay_us(uint32_t xus)
{
  int j;
  for (j = xus; j > 0; j--)
    ;
}
