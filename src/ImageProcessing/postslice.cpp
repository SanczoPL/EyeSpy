#include "postslice.h"

constexpr auto SLICETHREAD{ "SliceThread" };

PostSlice::PostSlice(QJsonObject const &a_config)
  : m_sliceThreadMax{ a_config[SLICETHREAD].toInt() }
  , m_counter(0)
  , m_addingCounter(0)
{
  for (quint32 i = 0; i < m_sliceThreadMax; i++)
  {
    cv::Mat image;
    v_mat.push_back(image);
  }
}

void PostSlice::onFromSlice(quint32 sliceNumber, cv::Mat image)
{
  H_Logger->trace("PostSlice::onFromSlice()sliceNumber:{} ", sliceNumber);
  H_Logger->trace("{}x{}x{} ", image.cols, image.rows, image.channels());
  v_mat[sliceNumber] = image;
  H_Logger->trace("PostSlice::onFromSlice()sliceNumber:{} ", sliceNumber);
}

void PostSlice::onUpdate()
{
  m_counter++;
  QTime myTimer;
  myTimer.start();

  H_Logger->trace("PostSlice::onUpdate() ");
  cv::Mat image = v_mat[0];
  for (quint32 i = 1; i < v_mat.size(); i++)
  {
    cv::vconcat(image, v_mat[i], image);
  }
  H_Logger->trace("PostSlice::showImagesOpenCV3() ");
  emit(showImagesOpenCV3(image, "slicesd"));

  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_addingCounter += nMilliseconds2;
  if (m_counter >= 100)
  {
    H_Logger->warn("PostSlice time:{}", ((double)m_addingCounter / (double)m_counter));
    m_addingCounter = 0;
    m_counter = 0;
  }
}
