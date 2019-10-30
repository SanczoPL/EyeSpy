#include "boundariesfinder.h"

constexpr auto OPENCV_PREVIEW{ "OpenCV_Preview" };
constexpr auto DECISIONTHRESHOLD{ "DecisionThreshold" };
constexpr auto WIDTH{ "Width" };
constexpr auto HEIGHT{ "Height" };
constexpr auto VOTE{ "vote" };

BoundariesFinder::BoundariesFinder(QJsonObject const &a_config)
  : m_firstTime(true)
  , m_counter(0)
  , m_addingCounter(0)
  , m_showDebugView{ a_config[OPENCV_PREVIEW].toBool() }
  , m_areaAcc(0)
  , m_thresh{ a_config[DECISIONTHRESHOLD].toInt() }
  , m_width{ a_config[WIDTH].toInt() }
  , m_height{ a_config[HEIGHT].toInt() }
  , m_vote{ a_config[VOTE].toDouble() }
  , m_cameraCounter(0)
  , m_debugPreview(false)
  , m_debugFrames(30)
  , m_debugFramesColor(30)
{
}
void BoundariesFinder::configure(QJsonObject const &a_config)
{
  H_Logger->trace("BoundariesFinder::configure()");
  m_showDebugView = { a_config[OPENCV_PREVIEW].toBool() };
  m_thresh = { a_config[DECISIONTHRESHOLD].toInt() };
  m_width = { a_config[WIDTH].toInt() };
  m_height = { a_config[HEIGHT].toInt() };
  m_vote = { a_config[VOTE].toDouble() };

  H_Logger->trace("BoundariesFinder::configure() m_showDebugView:{}", m_showDebugView);
  H_Logger->trace("BoundariesFinder::configure() m_thresh:{}", m_thresh);
  H_Logger->trace("BoundariesFinder::configure() m_width:{}", m_width);
  H_Logger->trace("BoundariesFinder::configure() m_height:{}", m_height);
  H_Logger->trace("BoundariesFinder::configure() m_vote:{}", m_vote);
}

void BoundariesFinder::showDebugView(cv::Mat &image) {}

void BoundariesFinder::firstTime(cv::Mat &image) {}

void BoundariesFinder::findCountour(cv::Mat &image)
{
  double area = 0.0;

  std::vector<std::vector<cv::Point>> contoursBEST;
  std::vector<cv::Vec4i> hierarchyBEST;

  H_Logger->trace("BoundariesFinder::findContours()");
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
  H_Logger->trace("BoundariesFinder::findContours() finitio contoursBEST.size:{}", contoursBEST.size());
  m_contours.swap(contoursBEST);
  m_hierarchy.swap(hierarchyBEST);
}

void BoundariesFinder::findOverlapping() {}

void BoundariesFinder::onUpdate(cv::Mat image)
{
  QTime myTimer;
  myTimer.start();
  m_cameraCounter++;

  /*
  if (m_firstTime)
  {
    firstTime(image);
  }
  */

  H_Logger->trace("findOverlapping");
  H_Logger->trace("BoundariesFinder::onUpdate() image.cols:{}", image.cols);
  H_Logger->trace("BoundariesFinder::onUpdate() image.rows:{}", image.rows);
  H_Logger->trace("BoundariesFinder::onUpdate() image.channel:{}", image.channels());

  int cols = image.cols;
  int rows = image.rows;

  cv::Mat image2 = image.clone();
  cv::resize(image2, image2, cv::Size(m_width, m_height), 0, 0, cv::INTER_CUBIC);
  // cv::resize(image2, image2, cv::Size(3, 3), 0, 0, cv::INTER_NEAREST);
  // cv::resize(image2, image2, cv::Size(m_width, m_height), 0, 0, cv::INTER_NEAREST);
  cv::threshold(image2, image2, m_thresh, 255, cv::THRESH_BINARY_INV);

  findCountour(image2);

  int middleCountour = 0;
  if (m_areaAcc > 0)
  {
    for (unsigned int i = 0; i < m_contours.size(); i++)
    {
      cv::Rect boundRect = cv::boundingRect(m_contours[i]);
      double area = cv::contourArea(m_contours[i]);
      int areaRatio = int(area / m_areaAcc * m_vote); // 10 głosów:

      middleCountour += (boundRect.x + (boundRect.width / 2)) * areaRatio;
    }
  }
  middleCountour = middleCountour / m_vote;
  double dir = ((middleCountour - (m_width / 2.0)) / 6.4);
  H_Logger->trace("BoundariesFinder::onUpdate() dir:{}", dir);
  cv::circle(image2, cv::Point(middleCountour, image2.rows / 2), 2, cv::Scalar(255), -1);
  cv::circle(image2, cv::Point(middleCountour, image2.rows / 2), 1, cv::Scalar(0), -1);

  double error = dir;
  if (error > 100.0)
  {
    error = 100.0;
  }
  if (error <= -100.0)
  {
    error = -100.0;
  }

  H_Logger->trace("emit(setActual((double)error)) m_debugPreview:{},m_cameraCounter:{},m_debugFrames:{}",
                  m_debugPreview, m_cameraCounter, m_debugFrames);
  emit(setActual((double)error));

  if (m_debugPreview)
  {
    if (m_cameraCounter > m_debugFrames)
    {
      cv::circle(image2, cv::Point(middleCountour, image2.rows / 2), 2, cv::Scalar(255), -1);
      cv::circle(image2, cv::Point(middleCountour, image2.rows / 2), 1, cv::Scalar(0), -1);
      QByteArray ImgByteI((char *)(image2.data), 768); // 32x24x1
      H_Logger->trace("emit(sendImage(ImgByteI));");
      emit(sendImage(ImgByteI));
      m_cameraCounter = 0;
    }
  }

  m_counter++;

  if (m_showDebugView)
  {
    emit(showImagesOpenCV3(image2, "BoundariesFinder"));
  }
  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_addingCounter += nMilliseconds2;
  if (m_counter >= 100)
  {
    H_Logger->info("(timer)BoundariesFinder time:{}", ((double)m_addingCounter / (double)m_counter));
    refreshIP(qint32(((double)m_addingCounter / (double)m_counter)));
    m_addingCounter = 0;
    m_counter = 0;
  }
}

void BoundariesFinder::onChangePreviewOnLine(QJsonObject obj)
{
  m_debugPreview = obj["preview"].toBool();
  m_debugFrames = obj["frames"].toInt();
  m_debugFramesColor = obj["framesColor"].toInt();
}
