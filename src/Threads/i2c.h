#ifndef I2C_H
#define I2C_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include "../I2C/PCA9685.h"
#include "../I2C/device.h"
#include "../tools.h"

// GPIO config
#define PWMA PCA_CHANNEL_0
#define AIN1 PCA_CHANNEL_1
#define AIN2 PCA_CHANNEL_2
#define PWMB PCA_CHANNEL_5
#define BIN1 PCA_CHANNEL_3
#define BIN2 PCA_CHANNEL_4

#define MOTORA 0
#define MOTORB 1

enum DIRec
{
  FORWARD = 1,
  BACKWARD = 0
};

class I2C : public QObject
{
  Q_OBJECT

 public:
  I2C(QJsonObject const &a_Configuration);
  int Motor_Init(void);
  int Servo_Init(void);

  void Motor_Run(UBYTE motor, DIRec dir, UWORD speed);

 public slots: // TODO:
  void onMotor_setPWMMotorAB(UWORD speedA, UWORD speedB, bool dirA, bool dirB);
  void onMotor_Stop();
  void onServo_setRotationAngle(UBYTE channel, UBYTE Angle);
  void onUpdate();

 signals:
  void startMotor();
  void startServo();

 private:
  PCA9685 *m_PCA9685;
  int m_motorFd;
  int m_servoFd;
};
#endif // I2C_H
