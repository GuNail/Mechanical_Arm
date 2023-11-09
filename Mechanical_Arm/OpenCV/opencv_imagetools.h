#ifndef OPENCV_IMAGETOOLS_H
#define OPENCV_IMAGETOOLS_H
#include "opencvhead.h"
#include <QImage>

QImage cvMat2Qimage(const cv::Mat &input);
cv::Mat Qimage2cvMat(QImage &input);
cv::Mat stackImages(const cv::Mat &img, const QList<int> &Arry);
cv::Mat cropContour(const cv::Mat &frame, const std::vector<cv::Point> &contour);
std::vector<cv::Point> findLargestContour(const std::vector<std::vector<cv::Point>> &contours);

#endif // OPENCV_IMAGETOOLS_H
