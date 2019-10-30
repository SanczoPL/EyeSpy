#ifndef POSTSLICE_H
#define POSTSLICE_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QTime>
#include <opencv2/opencv.hpp>
#include "../tools.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "slice.h"

class PostSlice : public QObject
{
  Q_OBJECT

 public:
  explicit PostSlice(QJsonObject const &a_config);

 signals:
  void showImagesOpenCV3(cv::Mat frame, std::string name);

 public slots:

 public slots:
  void onFromSlice(quint32 sliceNumber, cv::Mat image);
  void onUpdate();

 private:
  qint32 m_sliceThreadMax;
  std::vector<cv::Mat> v_mat;
  quint32 m_counter{};
  quint32 m_addingCounter{};
};

#endif // POSTSLICE_H
