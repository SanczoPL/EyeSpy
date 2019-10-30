#include "acquisition.h"

constexpr auto WIDTH{ "WidthAcquisition" };
constexpr auto HEIGHT{ "HeightAcquisition" };
constexpr auto COLOR_CODE{ "ColorCode" };
constexpr auto OPENCV_PREVIEW{ "OpenCV_Preview" };

class QJsonObject;

ImageAcquisition::ImageAcquisition(QJsonObject const &a_config)
  : m_width{ a_config[WIDTH].toInt() }
  , m_height{ a_config[HEIGHT].toInt() }
  , m_colorCode{ a_config[COLOR_CODE].toInt() }
  , m_counter(0)
  , m_addingCounter(0)
  , m_framerateAdd(0)
  , m_showDebugView{ a_config[OPENCV_PREVIEW].toBool() }
  , m_capture(new Capture(a_config))
  , m_debugPreview(false)
  , m_debugFrames(30)
  , m_debugFramesColor(30)
{
  H_Logger->trace("ImageAcquisition::ImageAcquisition()");
  configure(a_config);
}

ImageAcquisition::~ImageAcquisition()
{
  H_Logger->info("ImageAcquisition::~ImageAcquisition()");
  delete m_capture;
}

void ImageAcquisition::configure(QJsonObject const &a_config)
{
  H_Logger->trace("ImageAcquisition::configure()");
  m_width = { a_config[WIDTH].toInt() };
  m_height = { a_config[HEIGHT].toInt() };
  m_colorCode = { a_config[COLOR_CODE].toInt() };
  H_Logger->trace("ImageAcquisition::configure()");
  H_Logger->trace("ImageAcquisition::configure() m_width:{}", m_width);
  H_Logger->trace("ImageAcquisition::configure() m_height:{}", m_height);
  H_Logger->trace("ImageAcquisition::configure() m_colorCode:{}", m_colorCode);
}

void ImageAcquisition::onAddImage(cv::Mat image) {}

void ImageAcquisition::onUpdate()
{
  H_Logger->trace("ImageAcquisition::onAddImage()");
  m_counter++;
  m_cameraCounterColor++;
  m_framerate = 999;
  QTime myTimer;

  m_capture->grab(m_image);

  H_Logger->trace("ImageAcquisition::onAddImage() image.cols:{}", m_image.cols);
  H_Logger->trace("ImageAcquisition::onAddImage() image.rows:{}", m_image.rows);
  H_Logger->trace("ImageAcquisition::onAddImage() image.channel:{}", m_image.channels());

  if (m_image.empty() or m_image.cols == 0 or m_image.rows == 0)
  {
    H_Logger->trace("ImageAcquisition::onAddImage() m_capture failed capture a frame");
  }
  else
  {
    if (m_showDebugView)
    {
      emit(showImagesOpenCV3(m_image, "m_imageGrayResized"));
    }
    if (m_image.channels() == 3)
    {
      cvtColor(m_image, m_imageGray, m_colorCode);
    }
    else
    {
      m_imageGray = m_image.clone();
    }

    myTimer.start();
    cv::resize(m_imageGray, m_imageGrayResized, cv::Size(m_width, m_height));
    if (!m_imageGrayResized.empty())
    {
      m_framerate = static_cast<int>(1000.0 / m_lastFrameReciveTime.restart());
      H_Logger->trace("Recived new frame.framerate {}", m_framerate);

      H_Logger->trace("ImageAcquisition::onAddImage() emit(update(m_imageGrayResized))");

      H_Logger->trace("resize from {}x{}x{} to {}x{}x{}", m_image.cols, m_image.rows, m_image.channels(),
                      m_imageGrayResized.cols, m_imageGrayResized.rows, m_imageGrayResized.channels());
      emit(update(m_imageGrayResized));
    }
    H_Logger->trace("(configure)ImageAcquisition m_debugPreview:{},m_cameraCounterColor:{},m_debugFramesColor:{}",
                    m_debugPreview, m_cameraCounterColor, m_debugFramesColor);
    if (m_debugPreview and (m_cameraCounterColor > m_debugFramesColor))
    {
      QByteArray ImgByteI((char *)(m_image.data), 76800); // 320x240x1

      H_Logger->trace("(configure)ImageAcquisition emit(sendImage(ImgByteI));");
      emit(sendImageColor(ImgByteI));
      m_cameraCounterColor = 0;
    }
  }
  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_framerateAdd += m_framerate;
  m_addingCounter += nMilliseconds2;
  if (m_counter >= 500)
  {
    H_Logger->info("(timer)ImageAcquisition time:{}  framerate:{}", ((double)m_addingCounter / (double)m_counter),
                   m_framerateAdd / (double)m_counter);
    refreshAcc(qint32(((double)m_addingCounter / (double)m_counter)));
    m_addingCounter = 0;
    m_counter = 0;
    m_framerateAdd = 0;
  }
}

void ImageAcquisition::onChangePreviewOnLine(QJsonObject obj)
{
  m_debugPreview = obj["preview"].toBool();
  m_debugFramesColor = obj["framesColor"].toInt();
}

void ImageAcquisition::onChangeCapture(QJsonObject obj)
{
  H_Logger->trace("ImageAcquisition::onChangeCapture()");
  m_capture->changeCapture(obj);
}
