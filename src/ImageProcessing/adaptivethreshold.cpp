#include "adaptivethreshold.h"

constexpr auto AT_BLOCKSIZE{ "AT_BlockSize" };
constexpr auto AT_CONSTANT{ "AT_Constant" };
constexpr auto OPENCV_PREVIEW{ "OpenCV_Preview" };

AdaptiveThreshold::AdaptiveThreshold(QJsonObject const &a_config)
  : m_blockSize{ a_config[AT_BLOCKSIZE].toInt() }
  , m_constant{ a_config[AT_CONSTANT].toInt() }
  , m_counter(0)
  , m_addingCounter(0)
  , m_showDebugView{ a_config[OPENCV_PREVIEW].toBool() }
{
  H_Logger->trace("AdaptiveThreshold::AdaptiveThreshold() m_blockSize:{}", m_blockSize);
  H_Logger->trace("AdaptiveThreshold::AdaptiveThreshold() m_constant:{}", m_constant);
}

void AdaptiveThreshold::configure(QJsonObject const &a_config)
{
  H_Logger->trace("AdaptiveThreshold::configure()");
  m_blockSize = { a_config[AT_BLOCKSIZE].toInt() };
  m_constant = { a_config[AT_CONSTANT].toInt() };
  m_showDebugView = { a_config[OPENCV_PREVIEW].toBool() };
  H_Logger->trace("AdaptiveThreshold::configure() m_blockSize:{}", m_blockSize);
  H_Logger->trace("AdaptiveThreshold::configure() m_constant:{}", m_constant);
  H_Logger->trace("AdaptiveThreshold::configure() m_showDebugView:{}", m_showDebugView);
}

void AdaptiveThreshold::onUpdate(cv::Mat image)
{
  m_counter++;
  QTime myTimer;
  myTimer.start();

  H_Logger->trace("AdaptiveThreshold::onUpdate()");
  // TODO:: Fast adaptive!
  // adaptiveThreshold(image, image, 250, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, m_blockSize, m_constant);
  // cv::threshold(image, image, 100, 255, cv::THRESH_BINARY_INV);
  H_Logger->trace("emit(update(image))");
  emit(update(image));
  if (m_showDebugView)
  {
    emit(showImagesOpenCV3(image, "AdaptiveThreshold"));
  }

  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_addingCounter += nMilliseconds2;
  if (m_counter >= 100)
  {
    H_Logger->info("(timer)AdaptiveThreshold time:{}", ((double)m_addingCounter / (double)m_counter));
    m_addingCounter = 0;
    m_counter = 0;
  }
}
