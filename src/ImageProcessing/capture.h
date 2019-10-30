#ifndef CAPTURE_H
#define CAPTURE_H

#include <QJsonObject>
#include <QObject>
#include <QTime>
#include "../tools.h"
#include "raspicam_cv.h"
#include "raspicam_still_cv.h"
class Capture : public QObject
{
  Q_OBJECT
 public:
  Capture();
  ~Capture();
  explicit Capture(QJsonObject const &a_config);
  void confiugure(QJsonObject const &a_config);
  void loadCapture();
  void confiugureFirstTime(QJsonObject const &a_config);

  void grab(cv::Mat &image);
  bool isOpened() { return m_isOpened; }
  bool isGrabed() { return m_isGrabed; }

 signals:

 public slots:
  void changeCapture(QJsonObject const &a_config);

 private:
 private:
  bool m_rpiCamera;
  bool m_rpiCameraStill;
  qint32 m_width;
  qint32 m_height;

  qint32 m_brightness;
  qint32 m_contrast;

  qint32 m_saturation;
  qint32 m_gain;

 private:
  raspicam::RaspiCam_Cv *m_raspiCamera;
  raspicam::RaspiCam_Still_Cv *m_raspiCameraStill;
  cv::VideoCapture *m_USBCamera;

 private:
  cv::Mat m_image;
  bool m_isOpened;
  bool m_isGrabed;
};

#endif // CAPTURE_H
