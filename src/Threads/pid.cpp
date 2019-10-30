#include "pid.h"

ControlPID::ControlPID(QJsonObject const &a_config)
{
  H_Logger->trace("ControlPID::ControlPID()");
  ControlPID::cleanConfigure(a_config);
  onSetSetpoint(0.0);
}

ControlPID::~ControlPID()
{
  H_Logger->info("ControlPID::~ControlPID()");
  delete m_miniPID;
}

void ControlPID::configure(QJsonObject const &a_config)
{
  H_Logger->trace("ControlPID::configure()");

  delete m_miniPID;

  m_p = a_config["p"].toDouble();
  m_i = a_config["i"].toDouble();
  m_d = a_config["d"].toDouble();
  m_f = a_config["f"].toDouble();
  H_Logger->trace("ControlPID::MiniPID()");
  if (m_f == 0.0)
  {
    m_miniPID = new MiniPID(m_p, m_i, m_d);
  }
  else
  {
    m_miniPID = new MiniPID(m_p, m_i, m_d, m_f);
  }
  H_Logger->trace("ControlPID::configure() m_p:{}", m_p);
  H_Logger->trace("ControlPID::configure() m_i:{}", m_i);
  H_Logger->trace("ControlPID::configure() m_d:{}", m_d);
  H_Logger->trace("ControlPID::configure() m_f:{}", m_f);
}

void ControlPID::cleanConfigure(QJsonObject const &a_config)
{
  H_Logger->trace("ControlPID::configure()");

  m_p = a_config["p"].toDouble();
  m_i = a_config["i"].toDouble();
  m_d = a_config["d"].toDouble();
  m_f = a_config["f"].toDouble();
  H_Logger->trace("ControlPID::MiniPID()");
  if (m_f == 0.0)
  {
    m_miniPID = new MiniPID(m_p, m_i, m_d);
  }
  else
  {
    m_miniPID = new MiniPID(m_p, m_i, m_d, m_f);
  }
  H_Logger->trace("ControlPID::cleanConfigure() m_p:{}", m_p);
  H_Logger->trace("ControlPID::cleanConfigure() m_i:{}", m_i);
  H_Logger->trace("ControlPID::cleanConfigure() m_d:{}", m_d);
  H_Logger->trace("ControlPID::cleanConfigure() m_f:{}", m_f);
}

void ControlPID::onSetActual(double actual)
{
  m_actual = actual;
}

void ControlPID::onUpdate()
{
  double out = m_miniPID->getOutput(m_actual, m_setpoint);
  emit(update(out));
  H_Logger->trace("ControlPID::onUpdate() out:{}", out);
  emit(showDateFromPID(out, m_actual, m_setpoint));
}
void ControlPID::onSetSetpoint(double setpoint)
{
  m_setpoint = setpoint;
  m_miniPID->setSetpoint(m_setpoint);
  H_Logger->trace("ControlPID::onSetSetpoint()");
}

void ControlPID::onSetPID(double p, double i, double d)
{
  m_miniPID->setPID(p, i, d);
  H_Logger->trace("ControlPID::onSetPID()");
}

void ControlPID::onSetPID(double p, double i, double d, double f)
{
  m_miniPID->setPID(p, i, d, f);
  H_Logger->trace("ControlPID::onSetPID()");
}

void ControlPID::onSetMaxIOutput(double maximum)
{
  m_miniPID->setMaxIOutput(maximum);
  H_Logger->trace("ControlPID::onSetMaxIOutput()");
}

void ControlPID::onSetOutputLimits(double output)
{
  m_miniPID->setOutputLimits(output);
  H_Logger->trace("ControlPID::onSetOutputLimits()");
}

void ControlPID::onGetOutput()
{
  m_miniPID->getOutput();
  H_Logger->trace("ControlPID::onGetOutput()");
}

void ControlPID::onGetOutput(double actual)
{
  m_miniPID->getOutput(actual);
  H_Logger->trace("ControlPID::onGetOutput()");
}

void ControlPID::onReset()
{
  m_miniPID->reset();
  H_Logger->trace("ControlPID::onReset()");
}

void ControlPID::onSetOutputRampRate(double rate)
{
  m_miniPID->setOutputRampRate(rate);
  H_Logger->trace("ControlPID::onReset()");
}
