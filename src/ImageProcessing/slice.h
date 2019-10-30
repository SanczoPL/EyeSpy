#ifndef SLICE_H
#define SLICE_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QTime>
#include <opencv2/opencv.hpp>
#include "../tools.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class Slice : public QObject
{
  Q_OBJECT
 public:
  explicit Slice(QObject *parent = nullptr);

 signals:
  void showImagesOpenCV3(cv::Mat frame, std::string name);
  void fromSlice(quint32 sliceNumber, cv::Mat image);

 public slots:
  void onUpdate();
  void addToSlice(quint32 sliceNumber, cv::Mat image);
  void addToSliceOld(quint32 sliceNumber, cv::Mat image);

 private:
  std::vector<cv::Point> m_contourMax;
  quint32 m_counter{};
  quint32 m_addingCounter{};
};

#endif // SLICE_H
