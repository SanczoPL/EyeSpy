#include "preslice.h"
constexpr auto SLICETHREAD{ "SliceThread" };

PreSlice::PreSlice(QJsonObject const &a_config, std::vector<Slice *> slice)
  : m_sliceThreadMax{ a_config[SLICETHREAD].toInt() }
  , v_slice{ slice }
  , m_counter(0)
  , m_addingCounter(0)
{
  connect(this, &PreSlice::addToSliceProxy, this, &PreSlice::addToSlice);
}

void PreSlice::onUpdate(cv::Mat image)
{
  m_counter++;
  QTime myTimer;
  myTimer.start();

  H_Logger->trace("PreSlice::onUpdate()");
  // Algorithm:
  if (!image.empty())
  {
    qint32 sl = (qint32)(image.rows / (double)m_sliceThreadMax);
    for (qint32 i = 0; i < m_sliceThreadMax; i++)
    {
      qint32 part = sl * i;
      H_Logger->trace("PreSlice:i:{},image.rows:{},m_sliceThreadMax:{} ", i, image.rows, m_sliceThreadMax);
      emit(addToSliceProxy(i, image(cv::Rect(0, part, image.cols, sl))));
    }
  }

  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_addingCounter += nMilliseconds2;

  if (m_counter >= 100)
  {
    H_Logger->warn("PreSlice time:{}", ((double)m_addingCounter / (double)m_counter));
    m_addingCounter = 0;
    m_counter = 0;
  }
}

void PreSlice::addToSlice(quint32 sliceNumber, cv::Mat image)
{
  v_slice[sliceNumber]->addToSlice(sliceNumber, image);
}
