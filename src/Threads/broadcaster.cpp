#include "broadcaster.h"
#include <QDebug>

auto constexpr MY_ID{ SID("Rpi2") };

Broadcaster::Broadcaster(QJsonObject const &a_config)
  : IP("192.168.100.213")
  , PORT(12345)
{
  H_Logger->trace("Broadcaster::Broadcaster");
  configure(a_config);
}

Broadcaster::~Broadcaster() {}

void Broadcaster::configure(QJsonObject const &a_config)
{
  H_Logger->info("Broadcaster::configure");
  // UCQT::
  QObject::connect(this, &Broadcaster::ConnectUniCom, this, &Broadcaster::onConnectUniCom);
  QObject::connect(this, &Broadcaster::subscribeRequest, &m_IO, &UCQt::onSubscribe);
  QObject::connect(this, &Broadcaster::unsubscribeRequest, &m_IO, &UCQt::onUnsubscribe);
  QObject::connect(&m_IO, &UCQt::newMessage, this, &Broadcaster::onNewMessage);
  connect(this, &Broadcaster::sendMessageRequest2, &m_IO, &UCQt::onSendMessage);

  H_Logger->trace("Communication:");

  IP = (a_config["IP"].toString());
  PORT = (static_cast<quint16>(a_config["Port"].toInt()));
  H_Logger->warn("IP:{}:PORT:{}", IP.toStdString(), QString::number(PORT).toStdString());
  emit(ConnectUniCom());
  H_Logger->trace("device:");
  // device:
  m_deviceInfoTopicSID = SID(a_config["Rpi2_InfoTopic"].toString().toStdString().c_str());
  m_deviceVideoTopicSID = SID(a_config["Rpi2_VideoTopic"].toString().toStdString().c_str());
  m_deviceVideoTopicSIDColor = SID(a_config["Rpi2_VideoColorTopic"].toString().toStdString().c_str());
  m_deviceSyncTopicSID = SID(a_config["Rpi2_SyncTopic"].toString().toStdString().c_str());
  m_deviceCommandTopic = (a_config["Rpi2_CommandTopic"].toString());
  m_deviceCommandTopicSID = SID(m_deviceCommandTopic.toStdString().c_str());
  H_Logger->warn("Broadcaster::configure subscribeRequest info:",
                 a_config["Rpi2_InfoTopic"].toString().toStdString().c_str());
  // subscribeRequest Command Topic:
  QVector<QString> topics{};
  topics.push_back(m_deviceCommandTopic);
  emit(subscribeRequest(topics));

  connect(this, &Broadcaster::addTime, this, &Broadcaster::onAddTime);
}

void Broadcaster::onSendImage(QByteArray image)
{
  H_Logger->trace("Broadcaster::sendCommandJSON:onSendImage");
  UniMessage msg{ image };
  msg.fromData(image, UniMessage::BINARY, MY_ID, m_deviceVideoTopicSID);
  emit(sendMessageRequest2(msg.rawData()));
}

void Broadcaster::onSendImageColor(QByteArray image)
{
  H_Logger->trace("Broadcaster::sendCommandJSON:onSendImage");
  UniMessage msg{ image };
  msg.fromData(image, UniMessage::BINARY, MY_ID, m_deviceVideoTopicSIDColor);
  emit(sendMessageRequest2(msg.rawData()));
}

void Broadcaster::onSendCommandJSON(QJsonObject json) {}

void Broadcaster::onSendSyncJSON(QJsonObject json)
{
  UniMessage msg{};
  QByteArray stateData{ QJsonDocument{ json }.toJson(QJsonDocument::Compact) };
  msg.fromData(stateData, UniMessage::JSON, MY_ID, m_deviceSyncTopicSID);
  H_Logger->trace("Broadcaster::sendCommandJSON:m_deviceSyncTopicSID");
  emit(sendMessageRequest2(msg.rawData()));
}

void Broadcaster::onAddTime(QByteArray msg)
{
  UniMessage msg2{};
  // QByteArray stateData{ QJsonDocument{ json }.toJson(QJsonDocument::Compact) };
  msg2.fromData(msg, UniMessage::JSON, MY_ID, m_deviceInfoTopicSID);
  H_Logger->trace("Broadcaster::sendCommandJSON:m_deviceInfoTopicSID");
  emit(sendMessageRequest2(msg2.rawData()));
}

void Broadcaster::onSendInfoJSON(QJsonObject json)
{
  UniMessage msg{};
  QByteArray stateData{ QJsonDocument{ json }.toJson(QJsonDocument::Compact) };
  msg.fromData(stateData, UniMessage::JSON, MY_ID, m_deviceInfoTopicSID);
  H_Logger->trace("Broadcaster::sendCommandJSON:m_deviceInfoTopicSID");
  emit(sendMessageRequest2(msg.rawData()));
}

void Broadcaster::onConnectUniCom()
{
  m_IO.startConnection(IP, PORT);
}

void Broadcaster::onNewMessage(QByteArray const a_message)
{
  UniMessage msg{};
  bool b_valid_flag = false;
  bool ret = msg.parse(a_message);
  if (ret <= 0)
  {
    H_Logger->warn("msg not correct");
  }
  else
  {
    UniMessage::Header m_header = msg.header();

    H_Logger->trace("Broadcaster::onNewMessage() a_message.size():{} m_header.size:{}", a_message.size() - 20,
                    m_header.size);
    QByteArray m_content = msg.content();
    if (msg.isValid())
    {
      if (m_header.topic == m_deviceCommandTopicSID)
      {
        H_Logger->trace("m_deviceCommandTopicSID Broadcaster::onNewMessage():{}:{} m_header.topic:{}", m_header.topic,
                        m_content.toStdString(), m_header.topic);
        H_Logger->trace("m_deviceCommandTopicSID::onNewMessage():{}:{}", "m_deviceSyncTopicSID",
                        m_content.toStdString());
        H_Logger->trace("Broadcaster::onNewMessage()");
        // UniMessage message{};
        // message.parse(a_message);
        H_Logger->trace(" msg.header().size:{}", msg.header().size);
        H_Logger->trace(" sizeof (msg.header()):{}", sizeof(msg.header()));
        H_Logger->trace(" msg.header().prefix:{}", msg.header().prefix);
        H_Logger->trace(" msg.header().sender:{}", msg.header().sender);
        H_Logger->trace(" msg.header().topic:{}", msg.header().topic);
        H_Logger->trace("msg from Unicom:{}", msg.content().toStdString());
        H_Logger->trace(" msg.content().size():{}", msg.content().size());

        H_Logger->trace("msg from Unicom:{}", msg.content().toStdString());
        QJsonDocument jDoc;
        try
        {
          jDoc = { QJsonDocument::fromJson(msg.content()) };
        }
        catch (...)
        {
          H_Logger->warn("catch1");
        }

        try
        {
          if (!jDoc.isObject())
          {
            H_Logger->warn("Recived invalid Message");
          }
        }
        catch (...)
        {
          H_Logger->warn("catch2");
        }

        // qDebug() << "jDoc" << jDoc;
        auto const commandObject{ jDoc.object()["Command"].toObject() };
        auto const CMD_ID{ getObjectHash(commandObject, "CommandType") };
        QJsonObject Obj;
        switch (CMD_ID)
        {
          case SID("Control"):
            H_Logger->trace("Control");
            Obj = { jDoc.object()["Control"].toObject() };
            emit(addControl(Obj));
            break;
          case SID("State"):
            H_Logger->trace("State");
            Obj = { jDoc.object()["State"].toObject() };
            // qDebug() << "State recv:" << Obj << endl;
            emit(addState(Obj));
            break;
          case SID("PID"):
            H_Logger->trace("PID");
            Obj = { jDoc.object()["PID"].toObject() };
            // qDebug() << "PID recv:" << Obj << endl;
            emit(addPID(Obj));
            break;
          case SID("OpenCV_ImageAcquisition"):
            H_Logger->trace("OpenCV_ImageAcquisition configure");
            Obj = { jDoc.object()["OpenCV_ImageAcquisition"].toObject() };
            // qDebug() << "OpenCV_ImageAcquisition recv:" << Obj << endl;
            emit(addOpenCV_ImageAcquisition(Obj));
            break;
          case SID("Time"):
            Obj = { jDoc.object()["Time"].toObject() };
            // qDebug() << "obj recv:" << Obj << endl;
            emit(addTime(msg.content()));
            break;
          case SID("Preview"):
            H_Logger->trace("Preview");
            Obj = { jDoc.object()["Preview"].toObject() };
            // qDebug() << "Preview obj recv:" << Obj << endl;
            emit(changePreviewOnLine(Obj));
            break;
          case SID("Capture"):
            H_Logger->trace("Capture");
            Obj = { jDoc.object()["Capture"].toObject() };
            // qDebug() << "Capture obj recv:" << Obj << endl;
            emit(changeCapture(Obj));
            break;

          default: H_Logger->warn("Invalid command type"); break;
        }
      }
      else
      {
        if (b_valid_flag)
        {
          H_Logger->trace("Broadcaster::onNewMessage()");
        }
        else
        {
          H_Logger->trace("Broadcaster::onNewMessage() msg not supported");
        }
      }
    }
    else
    {
      H_Logger->warn("Broadcaster::onNewMessage() msg not valid");
    }
  }
}

hash_t Broadcaster::getObjectHash(QJsonObject const &a_object, QString const &a_key)
{
  if (a_object[a_key].isNull()) return SID("Invalid!");

  return SID(a_object[a_key].toString().toStdString().c_str());
}
