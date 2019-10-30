#include "motor.h"

Motor::~Motor() {}

Motor::Motor(QJsonObject const &a_config)
  : m_controlState(manual)
  , m_ms(0)
  , m_lastError(0)
  , m_motorExist(false)
{
  m_actualPWM.dirA = 0;
  m_actualPWM.dirB = 0;
  m_actualPWM.pwmA = 1;
  m_actualPWM.pwmB = 1;
  m_actualPWM.time = 200;

  H_Logger->trace("Motor::Motor()");
}

void Motor::configure(QJsonObject const &a_config) {}

void Motor::onStartMotor()
{
  H_Logger->info("Motor::onStartMotor()");
  m_motorExist = true;
}

void Motor::onUpdate()
{
  if (m_motorExist)
  {
    m_ms += 2;

    if (m_controlState == manual)
    {
      H_Logger->trace("Motor::manualState: m_ms:{}", m_ms);
      if (m_ms < m_actualPWM.time)
      {
      }
      else
      {
        if (d_controlPWM.size() > 0)
        {
          H_Logger->trace("Motor::d_controlPWM.size() > 0:{}", d_controlPWM.size());
          m_ms = 0;
          m_actualPWM = d_controlPWM[0];
          d_controlPWM.pop_front();
          emit(setPWMMotorAB(m_actualPWM.pwmA, m_actualPWM.pwmB, m_actualPWM.dirA, m_actualPWM.dirB));
        }
        else
        {
          stopMotors();
        }
      }
    }
    else if (m_controlState == automatic)
    {
      H_Logger->trace("Motor::automatic: m_ms:{}", m_ms);
      if (m_ms < m_actualPWM.time)
      {
      }
      else
      {
        // m_ms = 0;
        // m_motorDriver->setPWMMotorAB(m_actualPWM.pwmA, m_actualPWM.pwmB, m_actualPWM.dirA, m_actualPWM.dirB);
        stopMotors();
      }
    }
  }
}
void Motor::onAddControlFromPID(double error)
{
  if (m_lastError != error and m_motorExist)
  {
    m_lastError = error;

    H_Logger->trace("Motor::onAddControlFromPID() config() error:{}", error);
    controlPWM cS;
    cS.dirA = false;
    cS.dirB = false;
    if (error > 0)
    {
      cS.pwmA = 10;
      cS.pwmB = 10 + int(error);
    }
    else if (error < 0)
    {
      cS.pwmA = 10 - int(error);
      cS.pwmB = 10;
    }

    cS.time = 100;

    if (m_controlState == automatic)
    {
      onAddControlAuto(cS);
    }
  }
}

void Motor::onAddControl(QJsonObject obj)
{
  H_Logger->debug("Motor::onAddControl() configure() m_motorExist:{}", m_motorExist);
  controlPWM cS;
  cS.dirA = obj["dirA"].toBool();
  cS.dirB = obj["dirB"].toBool();
  cS.pwmA = obj["pwmA"].toInt();
  cS.pwmB = obj["pwmB"].toInt();
  cS.time = obj["time"].toInt();
  if (m_controlState == automatic and m_motorExist)
  {
    onAddControlAuto(cS);
  }
  if (m_controlState == manual and m_motorExist)
  {
    onAddControlManaual(cS);
  }
}

void Motor::stopMotors()
{
  H_Logger->trace("Motor::m_motorDriver->Motor_Stop();");
  if (m_actualPWM.pwmB == 0 and m_actualPWM.pwmA == 0)
  {
  }
  else
  {
    // Zatrzymaj silnik:!
    m_actualPWM.dirA = true;
    m_actualPWM.dirB = true;
    m_actualPWM.pwmA = 0;
    m_actualPWM.pwmB = 0;
    m_actualPWM.time = 10;
    m_ms = 0;
    emit(setPWMMotorAB(m_actualPWM.pwmA, m_actualPWM.pwmB, m_actualPWM.dirA, m_actualPWM.dirB));
  }
}

void Motor::onAddState(QJsonObject obj)
{
  H_Logger->trace("Motor::onAddState() configure()");
  onChangeControlState(obj["state"].toInt());
}

void Motor::onAddControlAuto(controlPWM cS)
{
  H_Logger->debug("Motor::onAddControlManaual: Silnik A w {} przez :{} ms z mocą:{}", cS.dirA, cS.time, cS.pwmA);
  H_Logger->debug("Motor::onAddControlManaual: Silnik B w {} przez :{} ms z mocą:{}", cS.dirB, cS.time, cS.pwmB);
  m_ms = 0;
  m_actualPWM = cS;
  emit(setPWMMotorAB(m_actualPWM.pwmA, m_actualPWM.pwmB, m_actualPWM.dirA, m_actualPWM.dirB));
}

void Motor::onAddControlManaual(controlPWM cS)
{
  H_Logger->debug("Motor::onAddControlManaual: Silnik A w {} przez :{} ms z mocą:{}", cS.dirA, cS.time, cS.pwmA);
  H_Logger->debug("Motor::onAddControlManaual: Silnik B w {} przez :{} ms z mocą:{}", cS.dirB, cS.time, cS.pwmB);
  d_controlPWM.push_back(cS);
}

void Motor::onChangeControlState(qint32 state)
{
  H_Logger->debug("(configure)Motor::onChangeControlState: state:{}", state);
  if (state == automatic)
  {
    m_controlState = automatic;
    emit(refreshMode("auto"));
  }
  else if (state == manual)
  {
    m_controlState = manual;
    emit(refreshMode("manual"));
  }
}
