#include "opencvthread.h"
#include <QMimeDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QPainter>

void OpenCVThread::run()
{
    qDebug()<<"video stream Thread runing"<<QThread::currentThread();

    if(!this->cameraDevice.isNull())
    {
        if(!(this->videoCapture.open(this->cameraDevice.toStdString())))
        {
            qDebug()<<"Open"<<this->cameraDevice<<"fail";
            emit openFail();
            return;
        }
        else
        {
            emit openSuccess();
        }

    }
    else if(this->deviceNum!=-1)
    {
        if(!this->videoCapture.open(this->deviceNum))
        {
            qDebug()<<"Open"<<this->deviceNum<<"fail";
            emit openFail();
            return;
        }
        else
        {
            emit openSuccess();
        }
    }

    while (this->runingFlag)
    {
        this->videoCapture.read(this->imageStream);
        if(!this->imageStream.empty())
        {
            emit sendImage(this->imageStream);
        }
        else
        {
            this->runingFlag=false;
            this->videoCapture.release();
            emit imageError();
            return;
        }
        QThread::msleep(30);
    }
    this->videoCapture.release();
}

OpenCVThread::OpenCVThread(QObject *parent):QThread{parent}
{

}

OpenCVThread::~OpenCVThread()
{

}

void OpenCVThread::setDevice(const QString &deviceName)
{
    this->cameraDevice=deviceName;
}

void OpenCVThread::setDevice(int num)
{
    this->deviceNum=num;
}



void OpenCVThread::player()
{
    if((!this->cameraDevice.isNull())||(this->deviceNum!=-1))
    {
        this->runingFlag=true;
    }
    else
    {
        qDebug()<<"Device is NULL!";
    }
}

void OpenCVThread::stop()
{
    this->runingFlag=false;
}

void OpenCVThread::close()
{
    this->cameraDevice=nullptr;
    this->runingFlag=false;
}


