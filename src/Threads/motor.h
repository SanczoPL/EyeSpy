#ifndef MOTOR_H
#define MOTOR_H

#include <signal.h>
#include <stdio.h>  //printf()
#include <stdlib.h> //exit()
#include <QJsonObject>
#include <QObject>
#include <deque>
#include "../I2C/PCA9685.h"
#include "../I2C/device.h"
#include "../tools.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

// enum DIRec;

enum controlState
{
  automatic = 0,
  manual = 1,
  calibration = 2
};

struct controlPWM
{
  qint32 pwmA;
  qint32 pwmB;
  qint32 time;
  bool dirA;
  bool dirB;
};

class Motor : public QObject
{
  Q_OBJECT

 public:
  Motor(QJsonObject const &a_Configuration);
  ~Motor();
  void configure(QJsonObject const &a_Configuration);

 private:
  void stopMotors();

 signals:
  void setPWMMotorAB(uint16_t speedA, uint16_t speedB, bool dirA, bool dirB);
  void refreshMode(QString mode);

 public slots:
  void onUpdate();
  void onAddControlAuto(controlPWM cS);
  void onAddControlManaual(controlPWM cS);
  void onChangeControlState(qint32 state);
  void onAddControl(QJsonObject obj);
  void onAddState(QJsonObject obj);
  void onAddControlFromPID(double error);
  void onStartMotor();

 private:
 private:
  controlState m_controlState;
  std::deque<controlPWM> d_controlPWM;
  qint16 deltaTime;
  controlPWM m_actualPWM;
  qint32 m_ms;
  double m_lastError;
  bool m_motorExist{};
};

#endif // MOTOR_H
