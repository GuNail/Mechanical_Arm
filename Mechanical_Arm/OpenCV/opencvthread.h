#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H

#include <QImage>
#include <QObject>
#include <QThread>
#include <QWidget>
#include "opencvhead.h"
#include "opencv_imagetools.h"


class OpenCVThread : public QThread
{
    Q_OBJECT

public:
    explicit OpenCVThread(QObject *parent = nullptr);
    ~OpenCVThread();

protected:
    void run();

signals:
    void sendImage(cv::Mat image);
    void openSuccess();
    void openFail();
    void imageError();

public slots:
    void setDevice(const QString &deviceName);
    void setDevice(int num);
    void player();
    void stop();
    void close();

private:

    volatile bool runingFlag=false;
    QString cameraDevice;
    int deviceNum=-1;
    cv::VideoCapture videoCapture;
    cv::Mat imageStream;
    QImage image;
};



#endif // OPENCVTHREAD_H
