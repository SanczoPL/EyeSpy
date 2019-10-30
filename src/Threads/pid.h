#ifndef CONTROLPID_H
#define CONTROLPID_H

#include <QJsonObject>
#include <QObject>
#include "../PID/MiniPID.h"
#include "../tools.h"

class ControlPID : public QObject
{
  Q_OBJECT

 public:
  ControlPID(QJsonObject const &a_config);
  ~ControlPID();
  void configure(QJsonObject const &a_config);
  void cleanConfigure(QJsonObject const &a_config);

 signals:
  void update(double out);
  void showDateFromPID(double out, double actual, double setpoint);

 public slots:
  void onSetActual(double actual);
  void onUpdate();
  void onSetSetpoint(double setpoint);
  void onSetPID(double p, double i, double d);
  void onSetPID(double p, double i, double d, double f);
  void onSetMaxIOutput(double maximum);
  void onSetOutputLimits(double output);
  void onGetOutput();
  void onGetOutput(double actual);
  void onReset();
  void onSetOutputRampRate(double rate);

 private:
  MiniPID *m_miniPID;

 private:
  double m_actual;
  double m_setpoint;

 private:
  double m_p{};
  double m_i{};
  double m_d{};
  double m_f{};
};

#endif // CONTROLPID_H
