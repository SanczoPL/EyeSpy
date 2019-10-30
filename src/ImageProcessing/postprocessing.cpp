#include "postprocessing.h"

ImagePostProcessing::ImagePostProcessing(QJsonObject const &a_config)
{
  H_Logger->trace("ImagePostProcessing::ImagePostProcessing()");
  cv::Mat image;
  // cv::VideoCapture cap(0);
}
void ImagePostProcessing::onUpdate(cv::Mat image)
{
  H_Logger->trace("ImageProcessing::onUpdate()");
  emit(showImagesOpenCV3(image, "median"));
}
