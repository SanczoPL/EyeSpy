#include "capture.h"

constexpr auto RPICAMERA{ "RpiCamera" };
constexpr auto RPICAMERASTILL{ "RpiCameraStill" };
constexpr auto WIDTH{ "WidthCamera" };
constexpr auto HEIGHT{ "HeightCamera" };
constexpr auto BRIGHTNESS{ "Brightness" };
constexpr auto CONTRAST{ "Contrast" };
constexpr auto SATURATION{ "Saturation" };
constexpr auto GAIN{ "Gain" };

Capture::Capture() {}

Capture::~Capture()
{
  if (m_rpiCamera and m_rpiCameraStill)
  {
    H_Logger->info("Capture::~Capture() destructor m_raspiCameraStill:");
    delete m_raspiCameraStill;
  }
  else if (m_rpiCamera)
  {
    H_Logger->info("Capture::~Capture() destructor m_raspiCamera:");
    delete m_raspiCamera;
  }
  else
  {
    H_Logger->info("Capture::~Capture() destructor m_USBCamera:");
    delete m_USBCamera;
  }
}

Capture::Capture(QJsonObject const &a_config)
  : m_rpiCamera{ a_config[RPICAMERA].toBool() }
  , m_rpiCameraStill{ a_config[RPICAMERASTILL].toBool() }
  , m_width{ a_config[WIDTH].toInt() }
  , m_height{ a_config[HEIGHT].toInt() }
  , m_brightness{ a_config[BRIGHTNESS].toInt() }
  , m_contrast{ a_config[CONTRAST].toInt() }
  , m_saturation{ a_config[SATURATION].toInt() }
  , m_gain{ a_config[GAIN].toInt() }
  , m_isOpened(false)
  , m_isGrabed(false)
{
  H_Logger->info("Capture::Capture() ");
  confiugureFirstTime(a_config);
  confiugure(a_config);
}
void Capture::confiugureFirstTime(QJsonObject const &a_config)
{
  if (m_rpiCamera and m_rpiCameraStill)
  {
    m_raspiCameraStill = new raspicam::RaspiCam_Still_Cv;
  }
  else if (m_rpiCamera)
  {
    H_Logger->trace("Capture::Capture() m_rpiCamera:");
    m_raspiCamera = new raspicam::RaspiCam_Cv();
  }
  else
  {
    H_Logger->trace("Capture::Capture() m_USBCamera:");
    m_USBCamera = new cv::VideoCapture();
  }
}

void Capture::confiugure(QJsonObject const &a_config)
{
  H_Logger->trace("Capture::confiugure() ");

  if (m_rpiCamera and m_rpiCameraStill)
  {
    if (m_raspiCameraStill->open())
    {
      m_isOpened = true;
    }
    else
    {
      m_isOpened = false;
      H_Logger->trace("Capture::Capture() m_raspiCameraStill: failed to open device:");
    }
  }
  else if (m_rpiCamera)
  {
    H_Logger->trace("Capture::Capture() m_rpiCamera:");
    H_Logger->trace("Capture::configure() m_width:{}", m_width);
    H_Logger->trace("Capture::configure() m_height:{}", m_height);
    H_Logger->trace("Capture::configure() m_brightness:{}", m_brightness);
    H_Logger->trace("Capture::configure() m_contrast:{}", m_contrast);
    H_Logger->trace("Capture::configure() m_saturation:{}", m_saturation);
    H_Logger->trace("Capture::configure() m_gain:{}", m_gain);
    m_raspiCamera->set(cv::CAP_PROP_FRAME_WIDTH, m_width);
    m_raspiCamera->set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
    m_raspiCamera->set(cv::CAP_PROP_BRIGHTNESS, m_brightness);
    m_raspiCamera->set(cv::CAP_PROP_CONTRAST, m_contrast);
    m_raspiCamera->set(cv::CAP_PROP_SATURATION, m_saturation);
    m_raspiCamera->set(cv::CAP_PROP_GAIN, m_gain);
    m_raspiCamera->set(cv::CAP_PROP_FPS, 90);
    m_raspiCamera->set(cv::CAP_PROP_FORMAT, CV_8UC1);
    // m_raspiCamera->set(cv::CAP_PROP_EXPOSURE, getParamVal("-ss", argc, argv));
    if (m_raspiCamera->open())
    {
      m_isOpened = true;
    }
    else
    {
      H_Logger->trace("Capture::Capture() m_rpiCamera: failed to open device:");
    }
  }
  else
  {
    H_Logger->trace("Capture::Capture() m_USBCamera:");
    // m_USBCamera.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
    // m_USBCamera.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
    Capture::loadCapture();
  }
}

void Capture::grab(cv::Mat &image)
{
  H_Logger->trace("Capture::grab()");

  if (m_rpiCamera and m_rpiCameraStill)
  {
    if (m_raspiCameraStill->grab())
    {
      m_raspiCameraStill->retrieve(image);
      m_isGrabed = true;
    }
    else
    {
      H_Logger->trace("Capture::grab() m_raspiCameraStill.grab() retrieve error");
      m_isGrabed = false;
    }
  }
  if (m_rpiCamera)
  {
    if (m_raspiCamera->grab())
    {
      m_raspiCamera->retrieve(image);
      H_Logger->trace("Capture::grab() m_raspiCamera.grab() retrieve ok");
      m_isGrabed = true;
    }
    else
    {
      m_isGrabed = false;
      H_Logger->trace("Capture::grab() m_raspiCamera.grab() retrieve error");
    }
  }
  else
  {
    *m_USBCamera >> image;
    if (image.empty())
    {
      m_isGrabed = false;
      H_Logger->trace("Capture::grab() image.empty()");
    }
  }
}
void Capture::loadCapture()
{
  H_Logger->trace("Capture::loadCapture()");
  m_USBCamera->open("/media/data/raw_video/LineDataset/line1/input/%d.png");
  cv::Mat image;
  *m_USBCamera >> image;
  if (image.empty() == true)
  {
    for (int i = 0; i < 20; i++)
    {
      m_USBCamera->open(i);
      *m_USBCamera >> image;
      if (image.empty() == false)
      {
        break;
      }
    }
  }
}

void Capture::changeCapture(QJsonObject const &a_config)
{
  H_Logger->trace("Capture::changeCapture()");

  m_width = { a_config[WIDTH].toInt() };
  m_height = { a_config[HEIGHT].toInt() };
  m_brightness = { a_config[BRIGHTNESS].toInt() };
  m_contrast = { a_config[CONTRAST].toInt() };
  m_saturation = { a_config[SATURATION].toInt() };
  m_gain = { a_config[GAIN].toInt() };

  confiugure(a_config);
}
