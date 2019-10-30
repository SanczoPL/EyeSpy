#include "mainwindow.h"
#include <QCheckBox>
#include <QPainter>
#include <QPushButton>
#include "ui_mainwindow.h"

auto constexpr MY_ID{ SID("PC_VIEW") };

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_enableTimer(true)
{
  m_palleteDarkGreen.setColor(QPalette::WindowText, Qt::darkGreen);
  m_palleteDarkRed.setColor(QPalette::WindowText, Qt::darkRed);

  ui->setupUi(this);
  connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
  connect(&m_IO, &UCQt::connected, this, [this]() {
    ui->statusLabel->setText("Connected");
    ui->statusLabel->setPalette(m_palleteDarkGreen);
  });
  connect(&m_IO, &UCQt::disconnected, this, [this]() {
    ui->statusLabel->setText("Disconnected");
    ui->statusLabel->setPalette(m_palleteDarkRed);
  });
  connect(ui->disconnectButton, &QPushButton::clicked, this, [this](bool const a_true) {
    (void)a_true;
    m_IO.endConnection();
  });
  connect(ui->autoReconnectCheckBox, &QCheckBox::stateChanged, this, [this](int const a_state) {
    if (a_state == Qt::Checked)
    {
      m_IO.setAutoReconnect(true, 500);
    }
    else
    {
      m_IO.setAutoReconnect(false, 500);
    }
  });
  connect(ui->subscribeButton, &QPushButton::clicked, this, &MainWindow::onSubscribeButtonClicked);
  connect(ui->unsubscribeButton, &QPushButton::clicked, this, &MainWindow::onUnsubscribeButtonClicked);
  connect(this, &MainWindow::subscribeRequest, &m_IO, &UCQt::onSubscribe);
  connect(this, &MainWindow::unsubscribeRequest, &m_IO, &UCQt::onUnsubscribe);
  connect(&m_IO, &UCQt::newMessage, this, &MainWindow::onNewMessage);
  connect(this, &MainWindow::sendMessageRequest, &m_IO, &UCQt::onSendMessage);

  // Create the image widget
  imageWidget = new CVImageWidget();
  imageWidget2 = new CVImageWidget();
  // Load an image
  cv::Mat image = cv::imread("1.png", true);
  imageWidget->showImage(image);
  imageWidget2->showImage(image);
  ui->vL->addWidget(imageWidget);
  ui->vL_2->addWidget(imageWidget2);

  timer_latency = new QTimer(this);
  timer_latency->start(500);
  connect(timer_latency, SIGNAL(timeout()), this, SLOT(onLatency()));

  connect(ui->checkBox, &QCheckBox::stateChanged, this, [this](int const a_state) {
    if (a_state == Qt::Checked)
    {
      sendPreview(true, ui->portTextEdit_2->text());
    }
    else
    {
      sendPreview(false, "false");
    }
  });
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::onConnectButtonClicked(bool a_true)
{
  (void)a_true;
  QString const IP{ ui->ipTextEdit->text() };
  quint16 const PORT{ static_cast<quint16>(ui->portTextEdit->text().toInt()) };

  m_IO.startConnection(IP, PORT);
}

void MainWindow::onSubscribeButtonClicked(bool a_true)
{
  (void)a_true;
  QVector<QString> topics{};
  topics.push_back(ui->topicSubscribeTextEdit->text());
  topics.push_back(ui->topicSubscribeTextEdit_2->text());
  topics.push_back(ui->topicSubscribeTextEdit_3->text());

  QString const TOPIC{ ui->topicSubscribeTextEdit->text() };
  QString const TOPIC2{ ui->topicSubscribeTextEdit_2->text() };
  QString const TOPIC3{ ui->topicSubscribeTextEdit_3->text() };
  m_infoTopicSID = SID(TOPIC.toStdString().c_str());
  m_videoTopicSID = SID(TOPIC2.toStdString().c_str());
  m_videoTopicSIDColor = SID(TOPIC3.toStdString().c_str());

  emit(subscribeRequest(topics));
}

void MainWindow::onUnsubscribeButtonClicked(bool a_true)
{
  (void)a_true;
  QVector<QString> topics{};
  topics.push_back(ui->topicSubscribeTextEdit->text());
  emit(unsubscribeRequest(topics));
}
void MainWindow::onLatency()
{
  if (m_enableTimer)
  {
    QTime now{};
    now.start();
    m_enableTimer = false;
    QJsonObject Command = { { "CommandType", "Time" } };
    QJsonObject Control = { { "time", now.toString("hh:mm:ss.zzz") } };

    QJsonObject infoJSONOBJ = { { "Command", Command }, { "Time", Control } };
    QJsonDocument doc(infoJSONOBJ);
    QByteArray dataArray = doc.toJson();
    UniMessage msg{};

    qDebug() << "sending   msg.content():" << doc;
    qDebug() << "sending  ui->outcomeTopicTextEdit->text().toStdString():"
             << ui->outcomeTopicTextEdit->text().toStdString().c_str();
    msg.fromData(doc.toJson(QJsonDocument::Compact), UniMessage::JSON, MY_ID,
                 SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
    emit(sendMessageRequest(msg.rawData()));
    qDebug() << "Recived  m_infoTopicSID:" << m_infoTopicSID;
    qDebug() << "Recived  m_infoTopicSID:" << SID(ui->outcomeTopicTextEdit->text().toStdString().c_str());
  }
  else
  {
    ui->label_17->setPalette(m_palleteDarkRed);
    m_enableTimer = true;
    ui->label_17->setText(QString::number(500) + "+");
  }
}

void MainWindow::onNewMessage(QByteArray const a_message)
{
  UniMessage msg{};
  msg.parse(a_message);

  if (msg.topicSID() == m_infoTopicSID)
  {
    QJsonDocument jDoc;
    jDoc = { QJsonDocument::fromJson(msg.content()) };

    if (!jDoc.isObject())
    {
      qDebug("Recived invalid Message");
    }

    auto const commandObject{ jDoc.object()["Command"].toObject() };
    qDebug() << "commqDebugandObject" << commandObject;
    auto const CMD_ID{ getObjectHash(commandObject, "CommandType") };

    switch (CMD_ID)
    {
      case SID("Time"):
        QTime now{};
        now.start();
        QJsonObject Obj;
        Obj = { jDoc.object()["Time"].toObject() };
        qDebug() << "Obj" << Obj;
        QTime now2 = QTime::fromString(Obj["time"].toString());
        qint64 delta = now2.msecsTo(now);
        qDebug() << "now:" << now << " now2:" << now2 << " latency : " << delta;

        if (delta < 20)
        {
          ui->label_17->setPalette(m_palleteDarkGreen);
        }
        else
        {
          ui->label_17->setPalette(m_palleteDarkRed);
        }
        ui->label_17->setText(QString::number(delta));
        m_enableTimer = true;
        break;
    }
  }
  else if (msg.topicSID() == m_videoTopicSID)
  {
    QByteArray data = msg.content();
    cv::Mat decodedMat = cv::Mat(24, 32, CV_8UC1, data.data());
    cv::resize(decodedMat, decodedMat, cv::Size(320, 240), 0, 0, cv::INTER_NEAREST);
    imageWidget->showImage(decodedMat);
  }
  else if (msg.topicSID() == m_videoTopicSIDColor)
  {
    QByteArray data = msg.content();
    cv::Mat decodedMat = cv::Mat(240, 320, CV_8UC1, data.data());
    imageWidget2->showImage(decodedMat);
  }
  else
  {
    qDebug() << "not known topic:";
  }
}

hash_t MainWindow::getObjectHash(QJsonObject const &a_object, QString const &a_key)
{
  if (a_object[a_key].isNull()) return SID("Invalid!");

  return SID(a_object[a_key].toString().toStdString().c_str());
}

void MainWindow::onSendButtonClicked(bool a_true) {}

void MainWindow::on_up_clicked()
{
  on_manual_clicked();
  int time = ui->timeBox->text().toInt();
  int pwmMax = ui->PWMBox->text().toInt();
  QJsonObject toSendJSON;
  qDebug("WebApi::on_up_clicked ");

  QJsonObject Command = { { "CommandType", "Control" } };
  QJsonObject Control = {
    { "dirA", false }, { "dirB", false }, { "pwmA", pwmMax }, { "pwmB", pwmMax }, { "time", time },
  };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "Control", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::on_subscribeButton_3_clicked() {}

void MainWindow::on_manual_clicked()
{
  QJsonObject toSendJSON;
  qDebug("WebApi::on_manual_clicked ");

  QJsonObject Command = { { "CommandType", "State" } };
  QJsonObject Control = { { "state", 1 } };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "State", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::on_Auto_clicked()
{
  QJsonObject toSendJSON;
  qDebug("WebApi::on_manual_clicked ");

  QJsonObject Command = { { "CommandType", "State" } };
  QJsonObject Control = { { "state", 0 } };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "State", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::on_left_clicked()
{
  on_manual_clicked();
  int time = ui->timeBox->text().toInt();
  int pwmMax = ui->PWMBox->text().toInt();
  QJsonObject toSendJSON;
  qDebug("WebApi::on_up_clicked ");

  QJsonObject Command = { { "CommandType", "Control" } };
  QJsonObject Control = {
    { "dirA", false }, { "dirB", false }, { "pwmA", 0 }, { "pwmB", pwmMax }, { "time", time },
  };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "Control", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::on_right_clicked()
{
  on_manual_clicked();
  int time = ui->timeBox->text().toInt();
  int pwmMax = ui->PWMBox->text().toInt();
  QJsonObject toSendJSON;
  qDebug("WebApi::on_up_clicked ");

  QJsonObject Command = { { "CommandType", "Control" } };
  QJsonObject Control = {
    { "dirA", false }, { "dirB", false }, { "pwmA", pwmMax }, { "pwmB", 0 }, { "time", time },
  };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "Control", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::on_down_clicked()
{
  on_manual_clicked();
  int time = ui->timeBox->text().toInt();
  int pwmMax = ui->PWMBox->text().toInt();
  QJsonObject toSendJSON;
  qDebug("WebApi::on_up_clicked ");

  QJsonObject Command = { { "CommandType", "Control" } };
  QJsonObject Control = {
    { "dirA", true }, { "dirB", true }, { "pwmA", pwmMax }, { "pwmB", pwmMax }, { "time", time },
  };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "Control", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::on_stop_clicked()
{
  on_manual_clicked();
  int time = ui->timeBox->text().toInt();
  int pwmMax = ui->PWMBox->text().toInt();
  QJsonObject toSendJSON;
  qDebug("WebApi::on_up_clicked ");

  QJsonObject Command = { { "CommandType", "Control" } };
  QJsonObject Control = {
    { "dirA", true }, { "dirB", true }, { "pwmA", 0 }, { "pwmB", 0 }, { "time", time },
  };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "Control", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::on_PID_CLICK_clicked()
{
  double pid_p = ui->timeBox_2->text().toDouble();
  double pid_i = ui->timeBox_3->text().toDouble();
  double pid_d = ui->timeBox_4->text().toDouble();
  double pid_f = ui->timeBox_8->text().toDouble();

  QJsonObject toSendJSON;
  qDebug("WebApi::on_PID_CLICK_clicked ");

  QJsonObject Command = { { "CommandType", "PID" } };
  QJsonObject Control = {
    { "p", pid_p }, { "i", pid_i }, { "d", pid_d }, { "f", 0 }, { "time", pid_f },
  };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "PID", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::on_OPENCV_CLICK_clicked()
{
  int w = ui->timeBox_5->text().toInt();
  int h = ui->timeBox_6->text().toInt();

  QJsonObject toSendJSON;
  qDebug("WebApi::on_OPENCV_CLICK_clicked ");

  QJsonObject Command = { { "CommandType", "OpenCV_ImageAcquisition" } };
  QJsonObject Control = {
    { "WidthAcquisition", w }, { "HeightAcquisition", h }, { "ColorCode", 6 }, { "OpenCV_Preview", false }
  };

  QJsonObject infoJSONOBJ = { { "Command", Command }, { "OpenCV_ImageAcquisition", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}

void MainWindow::sendPreview(bool m_bool, QString string)
{
  qDebug("WebApi::sendPreview ");
  QJsonObject toSendJSON;
  QJsonObject Command;
  QJsonObject Control;
  QJsonObject infoJSONOBJ;

  if (m_bool)
  {
    Command = { { "CommandType", "Preview" } };
    Control = { { "preview", true },
                { "frames", ui->portTextEdit_2->text().toInt() },
                { "framesColor", ui->portTextEdit_3->text().toInt() } };
    infoJSONOBJ = { { "Command", Command }, { "Preview", Control } };
    QJsonDocument doc(infoJSONOBJ);
    QByteArray dataArray = doc.toJson();
    UniMessage msg{};
    msg.fromData(dataArray, UniMessage::JSON, SID("Tester"),
                 SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
    emit(sendMessageRequest(msg.rawData()));
  }
  else
  {
    Command = { { "CommandType", "Preview" } };
    Control = { { "preview", false },
                { "frames", ui->portTextEdit_2->text().toInt() },
                { "framesColor", ui->portTextEdit_3->text().toInt() } };
    infoJSONOBJ = { { "Command", Command }, { "Preview", Control } };
    QJsonDocument doc(infoJSONOBJ);
    QByteArray dataArray = doc.toJson();
    UniMessage msg{};
    msg.fromData(dataArray, UniMessage::JSON, SID("Tester"),
                 SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
    emit(sendMessageRequest(msg.rawData()));
  }
}

void MainWindow::on_OPENCV_CLICK_2_clicked()
{
  qDebug("WebApi::on_OPENCV_CLICK_2_clicked ");
  QJsonObject toSendJSON;
  QJsonObject Command;
  QJsonObject Control;
  QJsonObject infoJSONOBJ;

  Command = { { "CommandType", "Capture" } };
  Control = { { "WidthCamera", ui->timeBox_7->text().toInt() }, { "HeightCamera", ui->timeBox_9->text().toInt() },
              { "Brightness", ui->timeBox_10->text().toInt() }, { "Contrast", ui->timeBox_11->text().toInt() },
              { "Saturation", ui->timeBox_12->text().toInt() }, { "Gain", ui->timeBox_13->text().toInt() } };
  infoJSONOBJ = { { "Command", Command }, { "Capture", Control } };
  QJsonDocument doc(infoJSONOBJ);
  QByteArray dataArray = doc.toJson();
  UniMessage msg{};
  msg.fromData(dataArray, UniMessage::JSON, SID("Tester"), SID(ui->outcomeTopicTextEdit->text().toStdString().c_str()));
  emit(sendMessageRequest(msg.rawData()));
}
