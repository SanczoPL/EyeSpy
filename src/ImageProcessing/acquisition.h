#ifndef IMAGEACQUISITION_H
#define IMAGEACQUISITION_H

#include <QJsonObject>
#include <QObject>
#include <QTime>
#include <opencv2/opencv.hpp>
#include "../tools.h"
#include "capture.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class ImageAcquisition : public QObject
{
  Q_OBJECT
 public:
  ImageAcquisition(QJsonObject const &a_config);
  ~ImageAcquisition();
  void configure(QJsonObject const &a_config);

 signals:
  void update(cv::Mat image);
  void showImagesOpenCV3(cv::Mat frame, std::string name);
  void sendImage(QByteArray image);
  void sendImageColor(QByteArray image);
  void refreshAcc(qint32 imageAcc);

 public slots:
  void onUpdate();
  void onAddImage(cv::Mat image);
  void onChangePreviewOnLine(QJsonObject obj);
  void onChangeCapture(QJsonObject obj);

 private:
  cv::Mat m_image;
  cv::Mat m_imageGray;
  cv::Mat m_imageGrayResized;

 private:
  qint32 m_width;
  qint32 m_height;

 private:
  qint32 m_colorCode;
  QTime m_lastFrameReciveTime{};
  int m_framerate{};
  qint32 m_framerateAdd{};
  quint32 m_counter{};
  quint32 m_addingCounter{};
  bool m_showDebugView;
  Capture *m_capture;

 private:
  bool m_debugPreview;
  qint32 m_debugFrames;
  qint32 m_debugFramesColor;
  qint32 m_cameraCounter;
  qint32 m_cameraCounterColor;
};

#endif // IMAGEACQUISITION_H
