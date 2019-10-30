#ifndef IMAGEPOSTPROCESSING_H
#define IMAGEPOSTPROCESSING_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include "../tools.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class ImagePostProcessing : public QObject
{
  Q_OBJECT

 public:
  ImagePostProcessing(QJsonObject const &a_config);

 signals:
  void showImagesOpenCV3(cv::Mat frame, std::string name);

 public slots:
  void onUpdate(cv::Mat image);
};

#endif // IMAGEPOSTPROCESSING_H
