#include "slice.h"

Slice::Slice(QObject *parent)
  : QObject(parent)
  , m_counter(0)
  , m_addingCounter(0)
{
}

void Slice::onUpdate()
{
  H_Logger->trace("Slice::onUpdate()");
}

void Slice::addToSliceOld(quint32 sliceNumber, cv::Mat image)
{
  H_Logger->trace("Slice::addToSlice():{}", sliceNumber);
  H_Logger->trace("{}x{}x{} ", image.cols, image.rows, image.channels());

  m_counter++;
  QTime myTimer;
  myTimer.start();
  cv::medianBlur(image, image, 5);
  double area = 0.0;
  double areaMax = 0.0;

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  H_Logger->trace("Slice::findContours()");
  // cv::findContours(image, contours, hierarchy, cv::RETR_EXTERNAL, cv::RETR_LIST, cv::Point(0, 0));
  cv::findContours(image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
  // cv::Mat img(500, 500, CV_8UC3);

  for (unsigned int i = 0; i < contours.size(); i++)
  {
    area = cv::contourArea(contours[i]);
    if (area >= (5000.0))
    {
      // create hull array for convex hull points
      std::vector<std::vector<cv::Point>> hull(contours.size());
      // for (int i = 0; i < contours.size(); i++)
      convexHull(cv::Mat(contours[i]), hull[i], false);
      cv::Mat drawing = cv::Mat::zeros(image.size(), CV_8UC3);

      cv::Scalar color_contours = cv::Scalar(0, 255, 0); // green - color for contours
      cv::Scalar color = cv::Scalar(255, 0, 0);          // blue - color for convex hull
      // draw ith contour
      drawContours(drawing, contours, i, color_contours, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
      // draw ith convex hull
      drawContours(drawing, hull, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
      emit(showImagesOpenCV3(drawing, "drawing"));
    }
  }

  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_addingCounter += nMilliseconds2;
  if (m_counter >= 100)
  {
    H_Logger->warn("Slice:{} time:{}", sliceNumber, ((double)m_addingCounter / (double)m_counter));
    m_addingCounter = 0;
    m_counter = 0;
  }
}

void Slice::addToSlice(quint32 sliceNumber, cv::Mat image)
{
  H_Logger->trace("Slice::addToSlice():{}", sliceNumber);
  H_Logger->trace("{}x{}x{} ", image.cols, image.rows, image.channels());

  cv::Rect up = cv::Rect(0, 0, image.cols, int(image.rows * 0.1));
  cv::Rect down = cv::Rect(0, int(image.rows * 0.9), image.cols, int(image.rows * 1));

  cv::Rect left = cv::Rect(0, 0, int(image.cols * 0.1), image.rows);
  cv::Rect right = cv::Rect(int(image.cols * 0.9), 0, image.cols, image.rows);

  m_counter++;
  QTime myTimer;
  myTimer.start();
  cv::medianBlur(image, image, 5);
  double area = 0.0;
  double areaMax = 0.0;

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  std::vector<std::vector<cv::Point>> contoursBEST;
  std::vector<cv::Vec4i> hierarchyBEST;

  H_Logger->trace("Slice::findContours()");
  cv::findContours(image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
  for (unsigned int i = 0; i < contours.size(); i++)
  {
    area = cv::contourArea(contours[i]);
    if (area >= (1.0))
    {
      contoursBEST.push_back(contours[i]);
      hierarchyBEST.push_back(hierarchy[i]);
    }
  }
  H_Logger->trace("Slice::findContours() finitio contoursBEST.size:{}", contoursBEST.size());
  contours.swap(contoursBEST);
  hierarchy.swap(hierarchyBEST);

  H_Logger->trace("Slice::findContours() finitio contours.size:{}", contours.size());
  if (contours.size() > 0)
  {
    quint32 contourMaxIter;

    for (unsigned int i = 0; i < contours.size(); i++)
    {
      area = cv::contourArea(contours[i]);
      if (area > areaMax)
      {
        areaMax = area;
        contourMaxIter = i;
      }
    }
    H_Logger->trace("Slice::addToSlice() contourMaxIter:{}", contourMaxIter);
    H_Logger->trace("Slice::addToSlice() areaMax:{}", areaMax);
    m_contourMax = contours[contourMaxIter];
    std::vector<cv::Point> prevContourMax = m_contourMax;

    std::vector<cv::Moments> muMax(1);
    std::vector<cv::Point2f> mcMax(1);

    muMax[0] = cv::moments(m_contourMax, false);
    mcMax[0] =
        cv::Point2f(static_cast<double>(muMax[0].m10 / muMax[0].m00), static_cast<double>(muMax[0].m01 / muMax[0].m00));

    std::vector<cv::Moments> muMaxPrev(1);
    std::vector<cv::Point2f> mcMaxPrev(1);

    muMaxPrev[0] = cv::moments(m_contourMax, false);
    mcMaxPrev[0] = cv::Point2f(static_cast<double>(muMaxPrev[0].m10 / muMaxPrev[0].m00),
                               static_cast<double>(muMaxPrev[0].m01 / muMaxPrev[0].m00));
    H_Logger->trace("Slice::addToSlice() mcMax[0].x:{}", mcMax[0].x);
    H_Logger->trace("Slice::addToSlice() mcMax[0].y:{}", mcMax[0].y);
    H_Logger->trace("Slice::addToSlice() mcMaxPrev[0].x:{}", mcMaxPrev[0].x);
    H_Logger->trace("Slice::addToSlice() mcMaxPrev[0].y:{}", mcMaxPrev[0].y);
    // mcMax[0].x
    if (mcMax[0].x > 0.0 and mcMax[0].y > 0.0)
    {
      if (mcMaxPrev[0].x > 0.0 and mcMaxPrev[0].y > 0.0)
      {
        if (qAbs(mcMax[0].x - mcMaxPrev[0].x) > 5 and qAbs(mcMax[0].y - mcMaxPrev[0].y) > 5)
        {
          H_Logger->trace("qAbs(mcMax[0].x - mcMaxPrev[0].x) > 5");
        }
        else
        {
          double area;
          cv::Rect boundRect;
          // for (unsigned int i = 0; i < contours.size(); i++)
          //{
          area = cv::contourArea(contours[contourMaxIter]);
          boundRect = cv::boundingRect(contours[contourMaxIter]);

          double overlappingMax = 0;
          int arenaMaxi = -10;
          int arenaMaxj = -10;
          double arena1Up = -10;
          double arena2Up = -10;
          int area_of_intersection_up = -10;
          area_of_intersection_up = (boundRect & up).area();
          arena1Up = boundRect.area();
          arena2Up = up.area();
          double ovelappingUp = (area_of_intersection_up / (arena1Up + arena2Up - area_of_intersection_up));
          std::string tempStr =
              "UP:" + std::to_string(ovelappingUp) + "  AreaOfInter:" + std::to_string(area_of_intersection_up);

          //}
          cvtColor(image, image, 8);
          cv::Scalar colorGreen = cv::Scalar(0, 255, 0);
          cv::drawContours(image, contours, static_cast<int>(contourMaxIter), cv::Scalar(0, 160, 0), 3, 4, hierarchy,
                           125, cv::Point());
          cv::circle(image, cv::Point(mcMax[0].x, image.rows / 2), 7, cv::Scalar(255, 150, 0), -1);
          cv::circle(image, cv::Point(image.cols / 2, image.rows / 2), 3, cv::Scalar(0, 0, 255), -1);

          cv::Mat copy = image.clone();
          cv::rectangle(copy, up, cv::Scalar(100, 100, 100, 100), -1);
          double alpha = 0.4;
          cv::addWeighted(image, alpha, copy, 1 - alpha, 0.0, image);

          cv::putText(image, tempStr, cv::Point2f(5, 15), cv::FONT_HERSHEY_COMPLEX_SMALL, 1,
                      cv::Scalar(255, 255, 255, 255));

          QString str = QString::number(sliceNumber) + "_slice";
          double dir = (double)(((image.cols / 2) - 320) / 10.0);
          H_Logger->trace("dir:{}", dir);
          emit(showImagesOpenCV3(image, str.toStdString()));
          emit(fromSlice(sliceNumber, image));
          // cv::imwrite(QString::number(m_counter).toStdString() + ".png", image);
        }
      }
    }
  }

  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_addingCounter += nMilliseconds2;
  if (m_counter >= 100)
  {
    H_Logger->warn("Slice:{} time:{}", sliceNumber, ((double)m_addingCounter / (double)m_counter));
    m_addingCounter = 0;
    m_counter = 0;
  }
}
