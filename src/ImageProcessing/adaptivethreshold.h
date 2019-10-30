#ifndef ADAPTIVETHRESHOLD_H
#define ADAPTIVETHRESHOLD_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QTime>
#include <opencv2/opencv.hpp>
#include "../tools.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class AdaptiveThreshold : public QObject
{
  Q_OBJECT
 public:
  explicit AdaptiveThreshold(QJsonObject const &a_config);
  void configure(QJsonObject const &a_config);

 signals:
  void update(cv::Mat image);
  void showImagesOpenCV3(cv::Mat frame, std::string name);

 public slots:
  void onUpdate(cv::Mat image);

 private:
  qint32 m_blockSize;
  qint32 m_constant;

  quint32 m_counter{};
  quint32 m_addingCounter{};
  bool m_showDebugView;
};

#endif // ADAPTIVETHRESHOLD_H
