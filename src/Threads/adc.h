#ifndef ANALOGREADWRITE_H
#define ANALOGREADWRITE_H

#include <pcf8591.h>
#include <wiringPi.h>
#include <QJsonObject>
#include <QObject>
#include <QTime>
#include "../tools.h"

class AnalogReadWrite : public QObject
{
  Q_OBJECT

 public:
  AnalogReadWrite(QJsonObject const &a_config);
  void configure(QJsonObject const &a_config);

 signals:
  void sendCommandJSON(QJsonObject json);
  void sendSyncJSON(QJsonObject json);
  void sendInfoJSON(QJsonObject json);
  void addState(QJsonObject obj);

 public slots:
  void onUpdate();

 private:
  volatile char counter;

 private:
  quint32 m_counter{};
  quint32 m_addingCounter{};
  quint32 m_counterA0{};
  quint32 m_counterA1{};
  quint32 m_counterA2{};
  quint32 m_counterA3{};
  QTime m_timerA0;
  QTime m_timerA1;
  QTime m_timerA2;
  QTime m_timerA3;
  bool m_LapMeterFlag;

 private:
  bool m_a0;
  bool m_a1;
  bool m_a2;
  bool m_a3;
};

#endif // ANALOGREADWRITE_H
