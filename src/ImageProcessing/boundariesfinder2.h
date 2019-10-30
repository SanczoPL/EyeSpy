#ifndef BoundariesFinder2_H
#define BoundariesFinder2_H

#include <QJsonObject>
#include <QObject>
#include <QTime>
#include <bitset>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../UniCom/ucqt.h"
#include "../tools.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class BoundariesFinder2 : public QObject
{
  Q_OBJECT
 public:
  explicit BoundariesFinder2(QJsonObject const &a_config);

 signals:
  void showImagesOpenCV3(cv::Mat frame, std::string name);
  void sendCommandJSON(QJsonObject json);
  void sendSyncJSON(QJsonObject json);
  void sendInfoJSON(QJsonObject json);
  void setActual(double actual);
  void sendImage(QByteArray image);

 public slots:

 public slots:
  void onUpdate(cv::Mat image);

 private:
  bool m_firstTime;
  bool m_showDebugView;

 private:
  quint32 m_counter{};
  quint32 m_addingCounter{};

 private:
  void firstTime(cv::Mat &image);
  void showDebugView(cv::Mat &image);
  void findCountour(cv::Mat &image);
  void findOverlapping();

 private:
  std::vector<std::vector<cv::Point>> m_contours;
  std::vector<cv::Vec4i> m_hierarchy;

  quint32 m_data;
  std::string m_binary;

  double m_areaAcc;
};

#endif // BoundariesFinder2_H
