#include "opencvwidget.h"

OpenCVWidget::OpenCVWidget(QWidget *parent) : QWidget(parent)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");

    this->thread = new OpenCVThread(this);

    connect(this->thread, &OpenCVThread::sendImage, this, [&](cv::Mat image) {
        this->imagestream = image;
        this->image = cvMat2Qimage(this->imagestream);
        this->image = this->image.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        updateimg();
    });

    connect(this->thread, &OpenCVThread::openSuccess, this, [=]() { emit openSuccess(); });

    connect(this->thread, &OpenCVThread::openFail, this, [=]() { emit openFail(); });

    connect(this->thread, &OpenCVThread::imageError, this, [=]() { emit imageError(); });

    this->image = QImage();
}

OpenCVWidget::~OpenCVWidget()
{
    close();
}

void OpenCVWidget::Capture()
{
    emit imageCaptured(this->image);
}

void OpenCVWidget::setCameraDevice(const QString &deviceName)
{
    if (!(deviceName.isNull()))
    {
        this->thread->setDevice(deviceName);
    }
    else
    {
        qDebug() << "no device!";
    }
}

void OpenCVWidget::setCameraDevice(int deviceNum)
{
    if (deviceNum != -1)
    {
        this->thread->setDevice(deviceNum);
    }
    else
    {
        qDebug() << "no device!";
    }
}

void OpenCVWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (this->image.isNull())
    {
        return;
    }
    // qDebug() << TIMEMS << "paintEvent" << objectName();
    QPainter painter(this);
    painter.drawImage(this->image.rect(), this->image);
}

void OpenCVWidget::updateimg()
{
    this->update();
}

void OpenCVWidget::open()
{
    clear();
    thread->player();
    thread->start();
}

void OpenCVWidget::close()
{
    if (thread->isRunning())
    {
        thread->stop();
        thread->quit();
        thread->wait();
    }
    QTimer::singleShot(1, this, [=]() { clear(); });
}

void OpenCVWidget::clear()
{
    this->image = QImage();
    update();
}
