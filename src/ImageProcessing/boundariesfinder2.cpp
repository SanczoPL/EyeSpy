#include "boundariesfinder2.h"

constexpr auto OPENCV_PREVIEW{ "OpenCV_Preview" };

BoundariesFinder2::BoundariesFinder2(QJsonObject const &a_config)
  : m_firstTime(true)
  , m_counter(0)
  , m_addingCounter(0)
  , m_showDebugView{ a_config["OpenCV"].toObject()[OPENCV_PREVIEW].toBool() }
  , m_areaAcc(0)
{
}

void BoundariesFinder2::showDebugView(cv::Mat &image) {}

void BoundariesFinder2::firstTime(cv::Mat &image) {}

void BoundariesFinder2::findCountour(cv::Mat &image)
{
  double area = 0.0;

  std::vector<std::vector<cv::Point>> contoursBEST;
  std::vector<cv::Vec4i> hierarchyBEST;

  H_Logger->trace("Slice::findContours()");
  cv::findContours(image, m_contours, m_hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

  m_areaAcc = 0;
  for (unsigned int i = 0; i < m_contours.size(); i++)
  {
    area = cv::contourArea(m_contours[i]);
    if (area >= (1.0))
    {
      contoursBEST.push_back(m_contours[i]);
      hierarchyBEST.push_back(m_hierarchy[i]);
      m_areaAcc += area;
    }
  }
  H_Logger->trace("Slice::findContours() finitio contoursBEST.size:{}", contoursBEST.size());
  m_contours.swap(contoursBEST);
  m_hierarchy.swap(hierarchyBEST);
}

void BoundariesFinder2::findOverlapping() {}

void BoundariesFinder2::onUpdate(cv::Mat image)
{
  QTime myTimer;
  myTimer.start();

  if (m_firstTime)
  {
    firstTime(image);
  }

  H_Logger->trace("findOverlapping");
  int cols = image.cols;
  int rows = image.rows;

  cv::Mat image2 = image.clone();
  cv::resize(image2, image2, cv::Size(32, 24), 0, 0, cv::INTER_CUBIC);
  // cv::resize(image2, image2, cv::Size(3, 3), 0, 0, cv::INTER_NEAREST);
  // cv::resize(image2, image2, cv::Size(32, 24), 0, 0, cv::INTER_NEAREST);
  cv::threshold(image2, image2, 100, 255, cv::THRESH_BINARY_INV);

  findCountour(image2);
  // findOverlapping();

  int middleCountour = 0;
  if (m_areaAcc > 0)
  {
    for (unsigned int i = 0; i < m_contours.size(); i++)
    {
      cv::Rect boundRect = cv::boundingRect(m_contours[i]);
      double area = cv::contourArea(m_contours[i]);
      int areaRatio = int(area / m_areaAcc * 10.0); // 10 głosów:

      middleCountour += (boundRect.x + (boundRect.width / 2)) * areaRatio;
    }
  }
  middleCountour = middleCountour / 10;
  double dir = ((middleCountour - 16.0) / 6.4);
  H_Logger->error("image.cols:{},dir:{}", image.cols, dir);

  cv::circle(image2, cv::Point(middleCountour, image2.rows / 2), 3, cv::Scalar(0), -1);
  cv::circle(image2, cv::Point(middleCountour, image2.rows / 2), 2, cv::Scalar(255), -1);
  cv::circle(image2, cv::Point(middleCountour, image2.rows / 2), 1, cv::Scalar(0), -1);

  QByteArray ImgByteI((char *)(image2.data), 768); // 32x24x1
  // UniMessage msg{ ImgByteI };
  // msg.fromData(ImgByteI, UniMessage::BINARY, MY_ID, m_senderVideoTopicSID);
  H_Logger->trace("emit(sendImage(ImgByteI));");
  emit(sendImage(ImgByteI));

  // cv::resize(image2, image2, cv::Size(cols, rows), 0, 0, cv::INTER_NEAREST);

  // cvtColor(image, image, 8);

  m_counter++;

  if (m_showDebugView)
  {
    emit(showImagesOpenCV3(image2, "BoundariesFinder2"));
  }
  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_addingCounter += nMilliseconds2;
  if (m_counter >= 100)
  {
    H_Logger->warn("BoundariesFinder2 time:{}", ((double)m_addingCounter / (double)m_counter));
    m_addingCounter = 0;
    m_counter = 0;
  }
}
