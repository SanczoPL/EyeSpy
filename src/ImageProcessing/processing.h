#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <opencv2/opencv.hpp>
#include "../tools.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class ImageProcessing : public QObject
{
  Q_OBJECT

 public:
  ImageProcessing();

 public slots:
  void onUpdate();
};

#endif // IMAGEPROCESSING_H
