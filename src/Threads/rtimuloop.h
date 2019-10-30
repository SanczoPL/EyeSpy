#ifndef RTIMULOOP_H
#define RTIMULOOP_H

#include <QJsonObject>
#include <QObject>
#include <QTime>
#include "../../RTIMULib2/RTIMULib/RTIMULib.h"
#include "../tools.h"

class RTIMULoop : public QObject
{
  Q_OBJECT

 public:
  RTIMULoop();
  ~RTIMULoop();
 signals:
  void update(RTVector3 data);
  void setActual(double actual);
  void showDateFromRTIMUL(RTVector3 data);
  void sendCommandJSON(QJsonObject json);
  void sendSyncJSON(QJsonObject json);
  void sendInfoJSON(QJsonObject json);

 public slots:
  void onUpdate();
  void onShowDate(RTVector3 data);

 private:
  RTIMUSettings *m_settings;
  RTIMU *m_imu;

 private:
  quint32 m_counter{};
  quint32 m_addingCounter{};
  bool m_imuExist{};
};

#endif // RTIMULOOP_H
