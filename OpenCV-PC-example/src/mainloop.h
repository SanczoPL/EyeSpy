#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <QObject>
#include <QThread>
#include <QTime>
#include <QTimer>
#include "../../src/ImageProcessing/acquisition.h"
#include "../../src/ImageProcessing/adaptivethreshold.h"
#include "../../src/ImageProcessing/boundariesfinder.h"
#include "../../src/broadcaster.h"
#include "../../src/tools.h"

class MainLoop : public QObject
{
  Q_OBJECT
 public:
  MainLoop(QJsonObject const &a_Configuration);
  ~MainLoop();

 private:
  QThread *m_workerImageAcquisition;
  QThread *m_workerImageAT;
  QThread *m_workerImageBF;
  QThread *m_workerBroadcaster;

  ImageAcquisition *m_imageAcquisition;
  AdaptiveThreshold *m_imageAT;
  BoundariesFinder *m_BF;
  Broadcaster *m_broadcaster;

 private:
  std::vector<QThread *> v_workerSlice;

 private:
  qint32 m_sliceThreadMax;

  QTimer *timer25FPS;
  long sysID;

 public slots:
  void onUpdate();
  void onShowImagesOpenCV3(cv::Mat frame, std::string name);

 private:
  bool m_showDebugView;
};

#endif // MAINLOOP_H
