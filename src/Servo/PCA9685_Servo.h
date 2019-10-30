#ifndef __PCA9685_SERVO_H_
#define __PCA9685_SERVO_H_

#include "../tools.h"
#include "DEV_Servo_Config.h"

#define SUBADR1 0x02
#define SUBADR2 0x03
#define SUBADR3 0x04
#define MODE1 0x00
#define MODE2 0x01
#define PRESCALE 0xFE
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09
#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

#define PWM_I2C_Addr 0x41
#define PWM_I2C_Hz 50

class PCA9685_Servo
{
 public:
  PCA9685_Servo();

  void PCA9685_setServoPulse(UBYTE channel, UWORD value);
  void Init_PCA9685();
  void PCA9685_setPWM(UBYTE channel, UWORD on, UWORD off);
  void PCA9685_Set_Rotation_Angle(UBYTE channel, UBYTE Angle);

 private:
  DEV_Servo_Config *m_devConfigServo;
};
#endif
