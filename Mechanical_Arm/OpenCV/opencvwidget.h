#ifndef OPENCVWIDGET_H
#define OPENCVWIDGET_H

#include "opencvhead.h"
#include "opencvthread.h"

#include <QCameraInfo>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QWidget>

class OpenCVWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit OpenCVWidget(QWidget *parent = nullptr);
    ~OpenCVWidget();

  signals:
    void imageCaptured(const QImage &image);
    void openSuccess();
    void openFail();
    void imageError();

  private:
    OpenCVThread *thread;
    QImage image;
    cv::Mat imagestream;

  public slots:
    void Capture(void);
    void setCameraDevice(const QString &deviceName);
    void setCameraDevice(int deviceNum);
    void open();
    void close();
    void clear();

  protected:
    void paintEvent(QPaintEvent *);

  private slots:
    void updateimg(void);
};
Q_DECLARE_METATYPE(QCameraInfo)
#endif // OPENCVWIDGET_H
