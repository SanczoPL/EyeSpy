#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QTextStream>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "../tools.h"

class SerialPort : public QObject
{
  Q_OBJECT
 public:
  SerialPort(QJsonObject const &a_config);
  ~SerialPort();

  void configure(QJsonObject const &a_config);
  int GetQueryMsg(unsigned char *buf, int start_buoy, int end_buoy);
  void handleError(QSerialPort::SerialPortError serialPortError);
  void handleReadyRead();
  void handleTimeout();
 private slots:
  void readData();

 private:
  QSerialPortInfo *m_serialInfo;
  QSerialPort *m_serialPort;

 private:
  unsigned short crc16_calc(unsigned char *buf, int bytes);
  double freq;
  QTextStream m_standardOutput;
  QByteArray m_readData;
  QTimer m_timer;
};

#endif // SERIALPORT_H
