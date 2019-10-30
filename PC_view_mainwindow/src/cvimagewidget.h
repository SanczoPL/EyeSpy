#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H

#include <QPainter>
#include <QWidget>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class CVImageWidget : public QWidget
{
  Q_OBJECT
 public:
  explicit CVImageWidget();

  QSize sizeHint() const { return _qimage.size(); }
  QSize minimumSizeHint() const { return _qimage.size(); }

 public slots:

  void showImage(const cv::Mat &image)
  {
    // Convert the image to the RGB888 format
    switch (image.type())
    {
      case CV_8UC1: cvtColor(image, _tmp, cv::COLOR_GRAY2RGB); break;
      case CV_8UC3: cvtColor(image, _tmp, cv::COLOR_BGR2RGB); break;
    }
    assert(_tmp.isContinuous());
    _qimage = QImage(_tmp.data, _tmp.cols, _tmp.rows, _tmp.cols * 3, QImage::Format_RGB888);

    this->setFixedSize(image.cols, image.rows);

    repaint();
  }

 protected:
  void paintEvent(QPaintEvent * /*event*/)
  {
    // Display the image
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), _qimage);
    painter.end();
  }

  QImage _qimage;
  cv::Mat _tmp;
};

#endif // CVIMAGEWIDGET_H
