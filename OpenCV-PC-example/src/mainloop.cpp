#include "mainloop.h"

constexpr auto SLICETHREAD{ "SliceThread" };
constexpr auto OPENCV_PREVIEW{ "OpenCV_Preview" };

MainLoop::~MainLoop() {}

MainLoop::MainLoop(QJsonObject const &a_Configuration)
  : m_sliceThreadMax{ a_Configuration[SLICETHREAD].toInt() }
  , m_showDebugView{ a_Configuration["MainLoop"].toObject()[OPENCV_PREVIEW].toBool() }
{
  H_Logger->trace("MainLoop::MainLoop()");

  // ImageAcquisition:
  m_imageAcquisition = new ImageAcquisition(a_Configuration["OpenCV_ImageAcquisition"].toObject());
  m_workerImageAcquisition = new QThread();
  m_imageAcquisition->moveToThread(m_workerImageAcquisition);
  connect(m_workerImageAcquisition, &QThread::finished, m_imageAcquisition, &QObject::deleteLater);
  m_workerImageAcquisition->start();

  // AT:
  H_Logger->trace("AT:");
  m_imageAT = new AdaptiveThreshold(a_Configuration["OpenCV_AdaptiveThreshold"].toObject());
  m_workerImageAT = new QThread();
  m_imageAT->moveToThread(m_workerImageAT);
  connect(m_workerImageAT, &QThread::finished, m_imageAT, &QObject::deleteLater);
  m_workerImageAT->start();
  connect(m_imageAcquisition, &ImageAcquisition::update, m_imageAT, &AdaptiveThreshold::onUpdate);

  H_Logger->debug("BoundariesFinder");
  m_BF = new BoundariesFinder(a_Configuration["OpenCV_BoundariesFinder"].toObject());
  m_workerImageBF = new QThread();
  m_BF->moveToThread(m_workerImageBF);
  connect(m_workerImageBF, &QThread::finished, m_BF, &QObject::deleteLater);
  m_workerImageBF->start();
  connect(m_imageAT, &AdaptiveThreshold::update, m_BF, &BoundariesFinder::onUpdate);


  m_broadcaster = new Broadcaster(a_Configuration["Communication"].toObject());
  m_workerBroadcaster = new QThread();
  m_broadcaster->moveToThread(m_workerBroadcaster);
  connect(m_workerBroadcaster, &QThread::finished, m_broadcaster, &QObject::deleteLater);
  m_workerBroadcaster->start();

  // Timers:
  timer25FPS = new QTimer(this);
  timer25FPS->start(40);

  // Signals:
  connect(timer25FPS, &QTimer::timeout, this, &MainLoop::onUpdate);
  connect(timer25FPS, SIGNAL(timeout()), m_imageAcquisition, SLOT(onUpdate()));

  H_Logger->debug("OpenCV Preview ");
  connect(m_imageAcquisition, &ImageAcquisition::showImagesOpenCV3, this, &MainLoop::onShowImagesOpenCV3);
  connect(m_imageAT, &AdaptiveThreshold::showImagesOpenCV3, this, &MainLoop::onShowImagesOpenCV3);
  connect(m_BF, &BoundariesFinder::showImagesOpenCV3, this, &MainLoop::onShowImagesOpenCV3);
}

void MainLoop::onUpdate()
{
  H_Logger->trace("MainLoop::onUpdate()");
}

void MainLoop::onShowImagesOpenCV3(cv::Mat frame, std::string name)
{
  if (m_showDebugView)
  {
    if ((frame.rows > 1) && (frame.cols > 1) && (!frame.empty()))
    {
      cv::imshow(name, frame);
      cv::waitKey(1);
    }
  }
}
