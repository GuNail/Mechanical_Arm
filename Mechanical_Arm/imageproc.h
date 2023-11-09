
#ifndef IMAGEPROC_H
#define IMAGEPROC_H

#include "OpenCV/opencv_imagetools.h"
#include <QDebug>
#include <QMultiMap>
#include <QObject>
#include <QThread>
#include <QImage>

class imageProcessor : public QObject
{
    Q_OBJECT
  public:
    explicit imageProcessor(QObject *parent = nullptr);

  public slots:
    void cvMatToQImage(const cv::Mat &image);
    void getThingsPosition(QImage &image);

  signals:
    void sendQimage(QImage image);
    void sendThingsPlace(QMultiMap<QString, QPointF>);

  private:
    void imageCorrect(cv::Mat &inputImage, cv::Mat &outputImage);
};

#endif // IMAGEPROC_H
