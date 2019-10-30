#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <stdio.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QThread>
#include <QVector>
#include "../UniCom/ucqt.h"
#include "../tools.h"

class Broadcaster : public QObject
{
  Q_OBJECT
 public:
  Broadcaster(QJsonObject const &a_broadcasterConfiguration);
  ~Broadcaster();

  void configure(QJsonObject const &a_broadcasterConfiguration);
  void process();

 private:
  UCQt m_IO{};
  hash_t getObjectHash(QJsonObject const &a_object, QString const &a_key);

 signals:
  void ConnectUniCom();
  void SubscribeData();
  void SubscribeSynch();
  void SubscribeCameras();
  void sendMessageRequest2(QByteArray const a_message);
  void addControl(QJsonObject obj);
  void addState(QJsonObject obj);
  void addPID(QJsonObject obj);
  void addTime(QByteArray msg);
  void addOpenCV_ImageAcquisition(QJsonObject obj);
  void changePreviewOnLine(QJsonObject obj);
  void changeCapture(QJsonObject obj);

 private slots:
  void onConnectUniCom();
  void onAddTime(QByteArray msg);

 signals:
  void subscribeRequest(QVector<QString> const a_topics);
  void unsubscribeRequest(QVector<QString> const a_topics);

 public slots:
  void onNewMessage(QByteArray const a_message);
  void onSendCommandJSON(QJsonObject json);
  void onSendSyncJSON(QJsonObject json);
  void onSendInfoJSON(QJsonObject json);
  void onSendImage(QByteArray image);
  void onSendImageColor(QByteArray image);

 signals:
  void updateTime(qint64 ts);

 private:
  QString IP{};
  quint16 PORT{};

 private:
  hash_t m_deviceInfoTopicSID{};
  hash_t m_deviceVideoTopicSID{};
  hash_t m_deviceVideoTopicSIDColor{};
  hash_t m_deviceDataTopicCh0SID{};
  hash_t m_deviceDataTopicCh1SID{};
  hash_t m_deviceSyncTopicSID{};
  QString m_deviceCommandTopic{};
  hash_t m_deviceCommandTopicSID{};

 private:
  qint64 m_timestampDevice{};
  qint64 m_timestampDeviceOld{};
  qint32 m_cameraNumber{};
};

#endif // BROADCASTER_H
