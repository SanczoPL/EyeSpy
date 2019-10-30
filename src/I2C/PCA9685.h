#ifndef PCA9685_H
#define PCA9685_H
#include "../tools.h"
#include "device.h"

// GPIO config
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

#define PCA_CHANNEL_0 0
#define PCA_CHANNEL_1 1
#define PCA_CHANNEL_2 2
#define PCA_CHANNEL_3 3
#define PCA_CHANNEL_4 4
#define PCA_CHANNEL_5 5
#define PCA_CHANNEL_6 6
#define PCA_CHANNEL_7 7
#define PCA_CHANNEL_8 8
#define PCA_CHANNEL_9 9
#define PCA_CHANNEL_10 10
#define PCA_CHANNEL_11 11
#define PCA_CHANNEL_12 12
#define PCA_CHANNEL_13 13
#define PCA_CHANNEL_14 14
#define PCA_CHANNEL_15 15

#define PWM_I2C_Hz 50

class PCA9685
{
 public:
  PCA9685();

  int PCA9685_Motor_Init(char addr);
  int PCA9685_Servo_Init(char addr);
  void PCA9685_Motor_SetPWMFreq(UWORD freq);
  void PCA9685_Servo_SetPWMFreq(UWORD freq);
  void PCA9685_Motor_SetPwmDutyCycle(UBYTE channel, UWORD pulse);
  void PCA9685_Servo_SetPwmDutyCycle(UBYTE channel, UWORD pulse);
  void PCA9685_Motor_SetLevel(UBYTE channel, UWORD value);

  void PCA9685_Motor_WriteByte(UBYTE reg, UBYTE value);
  void PCA9685_Servo_WriteByte(UBYTE reg, UBYTE value);
  UBYTE PCA9685_ReadByte(UBYTE reg);
  void PCA9685_Motor_SetPWM(UBYTE channel, UWORD on, UWORD off);
  void PCA9685_Servo_SetPWM(UBYTE channel, UWORD on, UWORD off);

  void PCA9685_Servo_setServoPulse(UBYTE channel, UWORD value);
  void PCA9685_Servo_setRotationAngle(UBYTE channel, UBYTE Angle);

 private:
  Device *m_device;
};

#endif /* PCA9685_H */
