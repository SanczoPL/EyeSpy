#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <QObject>
#include <QThread>
#include <QTime>
#include <QTimer>
#include "ImageProcessing/acquisition.h"
#include "ImageProcessing/adaptivethreshold.h"
#include "ImageProcessing/boundariesfinder.h"
#include "Threads/adc.h"
#include "Threads/broadcaster.h"
#include "Threads/epd.h"
#include "Threads/i2c.h"
#include "Threads/motor.h"
#include "Threads/pid.h"
#include "Threads/rtimuloop.h"
#include "Threads/serialport.h"
#include "raspicam_cv.h"
#include "tools.h"

class MainLoop : public QObject
{
  Q_OBJECT
 public:
  MainLoop(QJsonObject const &a_Configuration);
  void confiugure(QJsonObject const &a_config);
  void loadCapture();
  ~MainLoop();

 private:
  QThread *m_workerMotor;
  QThread *m_workerAnalogReadWrite;
  QThread *m_workerImageAcquisition;
  QThread *m_workerImageAT;
  QThread *m_workerImageBF;
  QThread *m_workerPID;
  QThread *m_workerRTI;
  QThread *m_workerBroadcaster;
  QThread *m_workerEPD;
  QThread *m_workerI2C;

  Motor *m_motor;
  AnalogReadWrite *m_analogReadWrite;
  ImageAcquisition *m_imageAcquisition;
  AdaptiveThreshold *m_imageAT;
  BoundariesFinder *m_BF;
  ControlPID *m_PID;
  RTIMULoop *m_RTI;
  SerialPort *m_serialPort;
  Broadcaster *m_broadcaster;
  EPD *m_EPD;
  I2C *m_I2C;

  long sysID;

 signals:
  void showImagesOpenCV3(cv::Mat frame, std::string name);
  void addImage(cv::Mat image);

 public slots:
  void onUpdate();
  void onShowImagesOpenCV3(cv::Mat frame, std::string name);
  void onShowDateFromRTIMUL(RTVector3 data);
  void onShowDateFromPID(double out, double actual, double setpoint);
  void onShowDate();

 private:
  double m_x;
  double m_out;
  double m_actual;
  double m_setpoint;
  RTVector3 m_data;
  bool m_showDebugView;

 private:
  qint32 m_TimerAcquisition;
  qint32 m_TimerAnalogRead;
  qint32 m_TimerRTI;
  qint32 m_TimerPID;
  qint32 m_TimerMotor;
  qint32 m_TimerEPD;

  QTimer *timer_Acquisition;
  QTimer *timer_AnalogRead;
  QTimer *timer_RTI;
  QTimer *timer_PID;
  QTimer *timer_Motor;
  QTimer *timer_EPD;

 private:
  cv::Mat m_image;
  cv::Mat m_imageGray;
  cv::Mat m_imageGrayResized;
  int m_framerate{};
  qint32 m_framerateAdd{};
  QTime m_lastFrameReciveTime{};
};

#endif // MAINLOOP_H
