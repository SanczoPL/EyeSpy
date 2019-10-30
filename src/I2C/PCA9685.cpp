#include "PCA9685.h"
#include <math.h>
#include <stdio.h>

PCA9685::PCA9685()
  : m_device(new Device())
{
  H_Logger->debug("PCA9685::PCA9685()");
}

void PCA9685::PCA9685_Motor_WriteByte(UBYTE reg, UBYTE value)
{
  m_device->Motor_I2C_WriteByte(reg, value);
}

void PCA9685::PCA9685_Servo_WriteByte(UBYTE reg, UBYTE value)
{
  m_device->Servo_I2C_WriteByte(reg, value);
}

UBYTE PCA9685::PCA9685_ReadByte(UBYTE reg)
{
  return m_device->Motor_I2C_ReadByte(reg);
}

void PCA9685::PCA9685_Motor_SetPWM(UBYTE channel, UWORD on, UWORD off)
{
  PCA9685_Motor_WriteByte(LED0_ON_L + 4 * channel, on & 0xFF);
  PCA9685_Motor_WriteByte(LED0_ON_H + 4 * channel, on >> 8);
  PCA9685_Motor_WriteByte(LED0_OFF_L + 4 * channel, off & 0xFF);
  PCA9685_Motor_WriteByte(LED0_OFF_H + 4 * channel, off >> 8);
}

void PCA9685::PCA9685_Servo_setServoPulse(UBYTE channel, UWORD value)
{
  value = value * 4096 / 20000;
  PCA9685_Servo_SetPWM(channel, 0, value);
}

void PCA9685::PCA9685_Servo_SetPWM(UBYTE channel, UWORD on, UWORD off)
{
  PCA9685_Servo_WriteByte(LED0_ON_L + 4 * channel, on & 0xFF);
  PCA9685_Servo_WriteByte(LED0_ON_H + 4 * channel, on >> 8);
  PCA9685_Servo_WriteByte(LED0_OFF_L + 4 * channel, off & 0xFF);
  PCA9685_Servo_WriteByte(LED0_OFF_H + 4 * channel, off >> 8);
}

void PCA9685::PCA9685_Servo_setRotationAngle(UBYTE channel, UBYTE Angle)
{
  UWORD temp;
  if (Angle < 0 && Angle > 180)
  {
    printf("Angle out of range \n");
  }
  else
  {
    temp = Angle * (2000 / 180) + 500;
    PCA9685_Servo_setServoPulse(channel, temp);
  }
}

int PCA9685::PCA9685_Motor_Init(char addr)
{
  H_Logger->debug("PCA9685::PCA9685_Servo_Init addr:{}:{:x}", addr, addr);
  int ret = m_device->Motor_I2C_Init(addr);
  H_Logger->debug("PCA9685::PCA9685_Init fd:{}", ret);
  if (ret <= 0)
  {
    H_Logger->debug("PCA9685::PCA9685_Init fd:{} failed", ret);
    return -1;
  }

  m_device->Motor_I2C_WriteByte(MODE1, 0x00);
  return ret;
}

int PCA9685::PCA9685_Servo_Init(char addr)
{
  H_Logger->debug("PCA9685::PCA9685_Motor_Init addr:{}:{:x}", addr, addr);
  int ret = m_device->Servo_I2C_Init(addr);
  H_Logger->debug("PCA9685::PCA9685_Servo_Init fd:{}", ret);
  if (ret <= 0)
  {
    H_Logger->debug("PCA9685::PCA9685_Servo_Init fd:{} failed", ret);
    return -1;
  }
  m_device->Servo_I2C_WriteByte(MODE1, 0x00);
  return ret;
}

void PCA9685::PCA9685_Motor_SetPWMFreq(UWORD freq)
{
  freq *= 0.9;
  double prescaleval = 25000000.0;
  prescaleval /= 4096.0;
  prescaleval /= freq;
  prescaleval -= 1;

  UBYTE prescale = floor(prescaleval + 0.5);

  UBYTE oldmode = PCA9685_ReadByte(MODE1);
  UBYTE newmode = (oldmode & 0x7F) | 0x10; // sleep

  PCA9685_Motor_WriteByte(MODE1, newmode);     // go to sleep
  PCA9685_Motor_WriteByte(PRESCALE, prescale); // set the prescaler
  PCA9685_Motor_WriteByte(MODE1, oldmode);
  m_device->Device_Delay_ms(5);
  PCA9685_Motor_WriteByte(MODE1, oldmode | 0x80); //  This sets the MODE1 register to turn on auto increment.
}

void PCA9685::PCA9685_Servo_SetPWMFreq(UWORD freq)
{
  freq *= 0.9;
  UDOUBLE prescaleval, oldmode;
  prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1.0;

  prescaleval = prescaleval + 3;

  oldmode = m_device->Servo_I2C_ReadByte(MODE1);
  m_device->Servo_I2C_WriteByte(MODE1, (oldmode & 0x7F) | 0x10);
  m_device->Servo_I2C_WriteByte(PRESCALE, prescaleval);
  m_device->Servo_I2C_WriteByte(MODE1, oldmode);
  m_device->Device_Delay_ms(200);
  m_device->Servo_I2C_WriteByte(MODE1, oldmode | 0x80);
  m_device->Servo_I2C_WriteByte(MODE2, 0x04);

  PCA9685_Servo_setServoPulse(0, 1500);
  PCA9685_Servo_setServoPulse(1, 1500);
}

void PCA9685::PCA9685_Motor_SetPwmDutyCycle(UBYTE channel, UWORD pulse)
{
  H_Logger->trace("PCA9685::PCA9685_SetPwmDutyCycle");
  PCA9685_Motor_SetPWM(channel, 0, pulse * (4096 / 100) - 1);
}

void PCA9685::PCA9685_Servo_SetPwmDutyCycle(UBYTE channel, UWORD pulse)
{
  H_Logger->trace("PCA9685::PCA9685_SetPwmDutyCycle");
  PCA9685_Servo_SetPWM(channel, 0, pulse * (4096 / 100) - 1);
}

void PCA9685::PCA9685_Motor_SetLevel(UBYTE channel, UWORD value)
{
  if (value == 1)
    PCA9685_Motor_SetPWM(channel, 0, 4095);
  else
    PCA9685_Motor_SetPWM(channel, 0, 0);
}
