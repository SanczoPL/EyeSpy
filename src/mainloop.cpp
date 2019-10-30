#include "mainloop.h"

constexpr auto OPENCV{ "OpenCV" };
constexpr auto OPENCV_PREVIEW{ "OpenCV_Preview" };
constexpr auto MAINLOOP{ "MainLoop" };
constexpr auto ACQUISITION{ "Acquisition" };
constexpr auto ANALOGREAD{ "AnalogRead" };
constexpr auto RTI{ "RTI" };
constexpr auto PID{ "PID" };
constexpr auto MOTOR{ "Motor" };
constexpr auto RPICAMERA{ "RpiCamera" };
constexpr auto RPICAMERASTILL{ "RpiCameraStill" };
constexpr auto WIDTH{ "WIDTH" };
constexpr auto HEIGHT{ "HEIGHT" };

MainLoop::~MainLoop()
{
  m_workerMotor->quit();
  m_workerAnalogReadWrite->quit();
  m_workerImageAcquisition->quit();
  m_workerImageAT->quit();
  m_workerImageBF->quit();
  m_workerPID->quit();
  m_workerRTI->quit();
  m_workerBroadcaster->quit();

  m_workerMotor->wait();
  m_workerAnalogReadWrite->wait();
  m_workerImageAcquisition->wait();
  m_workerImageAT->wait();
  m_workerImageBF->wait();
  m_workerPID->wait();
  m_workerRTI->wait();
  m_workerBroadcaster->wait();

  delete m_motor;
  delete m_analogReadWrite;
  delete m_imageAcquisition;
  delete m_imageAT;
  delete m_BF;
  delete m_PID;
  delete m_RTI;
  delete m_serialPort;
  delete m_broadcaster;
}

MainLoop::MainLoop(QJsonObject const &a_config)
  : m_showDebugView{ a_config[OPENCV].toObject()[OPENCV_PREVIEW].toBool() }
  , m_TimerAcquisition{ a_config[MAINLOOP].toObject()[ACQUISITION].toInt() }
  , m_TimerAnalogRead{ a_config[MAINLOOP].toObject()[ANALOGREAD].toInt() }
  , m_TimerRTI{ a_config[MAINLOOP].toObject()[RTI].toInt() }
  , m_TimerPID{ a_config[MAINLOOP].toObject()[PID].toInt() }
  , m_TimerMotor{ a_config[MAINLOOP].toObject()[MOTOR].toInt() }
{
  // EPD:
  /*
H_Logger->debug("MainLoop::MainLoop() EPD:");
m_EPD = new EPD(a_config);
m_workerEPD = new QThread();
m_EPD->moveToThread(m_workerEPD);
connect(m_workerEPD, &QThread::finished, m_EPD, &QObject::deleteLater);
m_workerEPD->start();
*/
  // SerialPort:
  /*
H_Logger->debug("MainLoop::MainLoop() SerialPort:");
m_serialPort = new SerialPort(a_config);*/

  // Motor:
  H_Logger->debug("MainLoop::MainLoop() Motor:");
  m_motor = new Motor(a_config);
  m_workerMotor = new QThread();
  m_motor->moveToThread(m_workerMotor);
  connect(m_workerMotor, &QThread::finished, m_motor, &QObject::deleteLater);
  m_workerMotor->start();

  // AnalogReadWrite:
  /*
  H_Logger->debug("MainLoop::MainLoop() AnalogReadWrite:");
  m_analogReadWrite = new AnalogReadWrite(a_config["Analog"].toObject());
  m_workerAnalogReadWrite = new QThread();
  m_analogReadWrite->moveToThread(m_workerAnalogReadWrite);
  connect(m_workerAnalogReadWrite, &QThread::finished, m_analogReadWrite, &QObject::deleteLater);
  m_workerAnalogReadWrite->start();*/

  // ImageAcquisition:
  H_Logger->debug("MainLoop::MainLoop() ImageAcquisition:");
  m_imageAcquisition = new ImageAcquisition(a_config["OpenCV_ImageAcquisition"].toObject());
  m_workerImageAcquisition = new QThread();
  m_imageAcquisition->moveToThread(m_workerImageAcquisition);
  connect(m_workerImageAcquisition, &QThread::finished, m_imageAcquisition, &QObject::deleteLater);
  m_workerImageAcquisition->start();
  H_Logger->trace("m_imageAcquisition: configure");
  m_imageAcquisition->configure(a_config["OpenCV_ImageAcquisition"].toObject());

  // AdaptiveThreshold:
  H_Logger->debug("MainLoop::MainLoop() AdaptiveThreshold:");
  m_imageAT = new AdaptiveThreshold(a_config["OpenCV_AdaptiveThreshold"].toObject());
  m_workerImageAT = new QThread();
  m_imageAT->moveToThread(m_workerImageAT);
  connect(m_workerImageAT, &QThread::finished, m_imageAT, &QObject::deleteLater);
  m_workerImageAT->start();

  // BoundariesFinder:
  H_Logger->debug("MainLoop::MainLoop() BoundariesFinder:");
  m_BF = new BoundariesFinder(a_config["OpenCV_BoundariesFinder"].toObject());
  m_workerImageBF = new QThread();
  m_BF->moveToThread(m_workerImageBF);
  connect(m_workerImageBF, &QThread::finished, m_BF, &QObject::deleteLater);
  m_workerImageBF->start();

  // Image processing connections:
  H_Logger->debug("MainLoop::MainLoop() Image processing connections:");
  connect(m_imageAcquisition, &ImageAcquisition::update, m_imageAT, &AdaptiveThreshold::onUpdate);
  connect(m_imageAT, &AdaptiveThreshold::update, m_BF, &BoundariesFinder::onUpdate);
  connect(this, &MainLoop::addImage, m_imageAcquisition, &ImageAcquisition::onAddImage);

  // PID:
  H_Logger->debug("MainLoop::MainLoop() PID:");
  m_PID = new ControlPID(a_config["PID"].toObject());
  m_workerPID = new QThread();
  m_PID->moveToThread(m_workerPID);
  connect(m_workerPID, &QThread::finished, m_workerPID, &QObject::deleteLater);
  m_workerPID->start();
  connect(m_BF, &BoundariesFinder::setActual, m_PID, &ControlPID::onSetActual);
  connect(m_PID, &ControlPID::update, m_motor, &Motor::onAddControlFromPID);

  // RTI:
  H_Logger->debug("MainLoop::MainLoop() RTI:");
  m_RTI = new RTIMULoop();
  m_workerRTI = new QThread();
  m_RTI->moveToThread(m_workerRTI);
  connect(m_workerRTI, &QThread::finished, m_workerRTI, &QObject::deleteLater);
  m_workerRTI->start();

  // I2C:
  H_Logger->debug("MainLoop::MainLoop() I2C:");
  m_I2C = new I2C(a_config);
  m_workerI2C = new QThread();
  m_I2C->moveToThread(m_workerI2C);
  connect(m_workerI2C, &QThread::finished, m_I2C, &QObject::deleteLater);
  m_workerI2C->start();

  // RTI to PID: Mock: TODO:
  // connect(m_RTI, &RTIMULoop::setActual, m_PID, &ControlPID::onSetActual);

  // Broadcaster:
  H_Logger->debug("MainLoop::MainLoop() Broadcaster:");
  m_broadcaster = new Broadcaster(a_config["Communication"].toObject());
  m_workerBroadcaster = new QThread();
  m_broadcaster->moveToThread(m_workerBroadcaster);
  connect(m_workerBroadcaster, &QThread::finished, m_broadcaster, &QObject::deleteLater);
  m_workerBroadcaster->start();
  connect(m_BF, &BoundariesFinder::sendImage, m_broadcaster, &Broadcaster::onSendImage);
  connect(m_imageAcquisition, &ImageAcquisition::sendImageColor, m_broadcaster, &Broadcaster::onSendImageColor);

  // sendInfoJSON
  H_Logger->debug("MainLoop::MainLoop() sendInfoJSON:");
  connect(m_RTI, &RTIMULoop::sendInfoJSON, m_broadcaster, &Broadcaster::onSendInfoJSON);
  // connect(m_analogReadWrite, &AnalogReadWrite::sendInfoJSON, m_broadcaster, &Broadcaster::onSendInfoJSON);
  connect(m_BF, &BoundariesFinder::sendInfoJSON, m_broadcaster, &Broadcaster::onSendInfoJSON);

  // Timers:
  H_Logger->debug("MainLoop::MainLoop() Timers:");
  H_Logger->trace("m_TimerAnalogRead:{}", m_TimerAnalogRead);
  H_Logger->trace("m_TimerAcquisition:{}", m_TimerAcquisition);
  H_Logger->trace("m_TimerRTI:{}", m_TimerRTI);
  H_Logger->trace("m_TimerPID:{}", m_TimerPID);
  H_Logger->trace("m_TimerMotor:{}", m_TimerMotor);

  timer_AnalogRead = new QTimer(this);
  timer_AnalogRead->start(m_TimerAnalogRead);

  timer_Acquisition = new QTimer(this);
  timer_Acquisition->start(m_TimerAcquisition);

  timer_PID = new QTimer(this);
  timer_PID->start(m_TimerPID);

  timer_RTI = new QTimer(this);
  timer_RTI->start(m_TimerRTI);

  timer_Motor = new QTimer(this);
  timer_Motor->start(m_TimerMotor);

  m_TimerEPD = 1000;
  timer_EPD = new QTimer(this);
  timer_EPD->start(m_TimerEPD);

  // Signals:
  H_Logger->debug("MainLoop::MainLoop() Signals:");
  connect(timer_Acquisition, &QTimer::timeout, this, &MainLoop::onUpdate);
  connect(timer_Motor, SIGNAL(timeout()), m_motor, SLOT(onUpdate()));
  // connect(timer_AnalogRead, SIGNAL(timeout()), m_analogReadWrite, SLOT(onUpdate()));
  connect(timer_Acquisition, SIGNAL(timeout()), m_imageAcquisition, SLOT(onUpdate()));
  connect(timer_PID, SIGNAL(timeout()), m_PID, SLOT(onUpdate()));
  // connect(timer_RTI, SIGNAL(timeout()), m_RTI, SLOT(onUpdate())); // 10ms
  // connect(timer_EPD, SIGNAL(timeout()), m_EPD, SLOT(onUpdate())); // 1s
  connect(timer_EPD, SIGNAL(timeout()), m_I2C, SLOT(onUpdate())); // 1s

  // ShowData:
  H_Logger->debug("MainLoop::MainLoop() Signals2:");
  // connect(m_RTI, &RTIMULoop::showDateFromRTIMUL, this, &MainLoop::onShowDateFromRTIMUL);
  connect(m_PID, &ControlPID::showDateFromPID, this, &MainLoop::onShowDateFromPID);

  connect(m_broadcaster, &Broadcaster::addControl, m_motor, &Motor::onAddControl);
  connect(m_broadcaster, &Broadcaster::addState, m_motor, &Motor::onAddState);
  // connect(m_analogReadWrite, &AnalogReadWrite::addState, m_motor, &Motor::onAddState);

  H_Logger->debug("MainLoop::MainLoop() Signals3:");
  // connect(m_broadcaster, &Broadcaster::addOpenCV_ImageAcquisition, m_imageAcquisition,
  // &ImageAcquisition::configure);
  connect(m_broadcaster, &Broadcaster::changePreviewOnLine, m_BF, &BoundariesFinder::onChangePreviewOnLine);
  connect(m_broadcaster, &Broadcaster::changePreviewOnLine, m_imageAcquisition,
          &ImageAcquisition::onChangePreviewOnLine);
  connect(m_broadcaster, &Broadcaster::addPID, m_PID, &ControlPID::configure);
  connect(m_broadcaster, &Broadcaster::changeCapture, m_imageAcquisition, &ImageAcquisition::onChangeCapture);
  H_Logger->debug("MainLoop::MainLoop() refresh EPD:");
  // connect(m_BF, &BoundariesFinder::refreshIP, m_EPD, &EPD::onRefreshIP);
  // connect(m_imageAcquisition, &ImageAcquisition::refreshAcc, m_EPD, &EPD::onRefreshAcc);
  // connect(m_motor, &Motor::refreshMode, m_EPD, &EPD::onRefreshMode);
  H_Logger->debug("MainLoop::MainLoop() I2C:");
  connect(m_I2C, &I2C::startMotor, m_motor, &Motor::onStartMotor);
  connect(m_motor, &Motor::setPWMMotorAB, m_I2C, &I2C::onMotor_setPWMMotorAB);
}

void MainLoop::confiugure(QJsonObject const &a_config)
{
  H_Logger->trace("Capture::confiugure() ");
}

void MainLoop::onUpdate() {}

void MainLoop::onShowImagesOpenCV3(cv::Mat frame, std::string name)
{
  if ((frame.rows > 1) && (frame.cols > 1) && (!frame.empty()))
  {
    cv::imshow(name, frame);
    cv::waitKey(1);
  }
}

void MainLoop::onShowDateFromRTIMUL(RTVector3 data)
{
  m_x = data.x();
  m_data = data;
  MainLoop::onShowDate();
}

void MainLoop::onShowDateFromPID(double out, double actual, double setpoint)
{
  m_out = out;
  m_actual = actual;
  m_setpoint = setpoint;
  MainLoop::onShowDate();
}

void MainLoop::onShowDate()
{
  H_Logger->trace("Sample rate set:{} act:{} out:{} x:{}", m_setpoint, m_actual, m_out, m_x);
}
