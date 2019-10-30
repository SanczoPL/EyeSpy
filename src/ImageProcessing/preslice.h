#ifndef PRESLICE_H
#define PRESLICE_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QTime>
#include <opencv2/opencv.hpp>
#include "../tools.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "slice.h"

class PreSlice : public QObject
{
  Q_OBJECT
 public:
  PreSlice(QJsonObject const &a_config, std::vector<Slice *> slice);

 signals:
  void addToSliceProxy(quint32 sliceNumber, cv::Mat image);

 public slots:
  void onUpdate(cv::Mat image);
  void addToSlice(quint32 sliceNumber, cv::Mat image);

 private:
  qint32 m_sliceThreadMax;
  std::vector<Slice *> v_slice;
  quint32 m_counter{};
  quint32 m_addingCounter{};
};

#endif // PRESLICE_H
