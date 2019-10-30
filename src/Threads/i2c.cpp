#include "i2c.h"

I2C::I2C(QJsonObject const &a_Configuration)
  : m_PCA9685(new PCA9685())
  , m_motorFd(-1)
  , m_servoFd(-1)
{
  H_Logger->debug("I2C::I2C()");
  m_motorFd = Motor_Init();
  if (m_motorFd > 0)
  {
    H_Logger->info("I2C::I2C::Motor_Init() success");
    // emit(startMotor());
  }
  else
  {
    H_Logger->error("I2C::I2C::Motor_Init()<0)");
  }
  // m_servoFd = Servo_Init();
  if (m_servoFd > 0)
  {
    H_Logger->info("I2C::I2C::Servo_Init() success");
    m_PCA9685->PCA9685_Servo_setRotationAngle(0, 80);
    /// emit(startServo());
  }
  else
  {
    H_Logger->error("I2C::I2C::Servo_Init()<0)");
  }
  H_Logger->debug("I2C::I2C() success");
}

void I2C::onUpdate()
{
  if (m_motorFd > 0)
  {
    H_Logger->info("I2C::I2C::Motor_Init() success");
    emit(startMotor());
  }
}

void I2C::onServo_setRotationAngle(UBYTE channel, UBYTE Angle)
{
  m_PCA9685->PCA9685_Servo_setRotationAngle(channel, Angle);
}

int I2C::Motor_Init(void)
{
  H_Logger->debug("I2C::Motor_Init");
  int ret = m_PCA9685->PCA9685_Motor_Init(0x50);
  H_Logger->debug("I2C::Motor_Init fd:{}", ret);
  if (ret < 0)
  {
    H_Logger->error("I2C::Motor_Init fd:{}", ret);
    return -1;
  }
  m_PCA9685->PCA9685_Motor_SetPWMFreq(100);
  H_Logger->debug("I2C::Motor_Init return");
  return ret;
}

int I2C::Servo_Init(void)
{
  H_Logger->debug("I2C::Servo_Init");
  int ret = m_PCA9685->PCA9685_Servo_Init(0x41);
  H_Logger->debug("I2C::Servo_Init fd:{}", ret);
  if (ret < 0)
  {
    H_Logger->error("I2C::Servo_Init fd:{}", ret);
    return -1;
  }
  m_PCA9685->PCA9685_Servo_SetPWMFreq(100);
  return ret;
}

void I2C::onMotor_Stop()
{
  H_Logger->trace("I2C::Motor_Stop");
  m_PCA9685->PCA9685_Motor_SetPwmDutyCycle(PWMA, 0);
  m_PCA9685->PCA9685_Motor_SetPwmDutyCycle(PWMB, 0);
}

void I2C::onMotor_setPWMMotorAB(UWORD speedA, UWORD speedB, bool dirA, bool dirB)
{
  H_Logger->trace("I2C::setPWMMotorAB dirA:{},PWMA:{},dirB:{},PWMB:{} ", dirA, speedA, dirB, speedB);
  if (speedA > 100) speedA = 100;
  if (speedA < 1) speedA = 1;

  if (speedB > 100) speedB = 100;
  if (speedB < 1) speedB = 1;

  H_Logger->trace("I2C::setPWMMotorAB");
  m_PCA9685->PCA9685_Motor_SetPwmDutyCycle(PWMA, speedA);
  if (dirA)
  {
    m_PCA9685->PCA9685_Motor_SetLevel(AIN1, 0);
    m_PCA9685->PCA9685_Motor_SetLevel(AIN2, 1);
  }
  else
  {
    m_PCA9685->PCA9685_Motor_SetLevel(AIN1, 1);
    m_PCA9685->PCA9685_Motor_SetLevel(AIN2, 0);
  }

  m_PCA9685->PCA9685_Motor_SetPwmDutyCycle(PWMB, speedB);
  if (dirB)
  {
    m_PCA9685->PCA9685_Motor_SetLevel(BIN1, 0);
    m_PCA9685->PCA9685_Motor_SetLevel(BIN2, 1);
  }
  else
  {
    m_PCA9685->PCA9685_Motor_SetLevel(BIN1, 1);
    m_PCA9685->PCA9685_Motor_SetLevel(BIN2, 0);
  }
}
