#include "SerialManage.h"
#include <QDebug>

SerialManage::SerialManage(QObject *parent): QObject{parent}
{
    serialPort =new QSerialPort(this);

    connect(this->serialPort,&QSerialPort::readyRead,
            this,&SerialManage::readData);
}

SerialManage::~SerialManage()
{
    serialPort->deleteLater();
}

void SerialManage::setSerialConfig(serialConfigStruct SerialConfig)
{
    this->serialConfig = SerialConfig;
}

void SerialManage::connectSerial(bool &isOpen)
{
    this->serialPort->setParity(this->serialConfig.vSerialParrity);
    this->serialPort->setPortName(this->serialConfig.vSerialPortName);
    this->serialPort->setBaudRate(this->serialConfig.vSerialBaudRate);
    this->serialPort->setDataBits(this->serialConfig.vSerialDataBits);
    this->serialPort->setStopBits(this->serialConfig.vSerialStopBits);
    this->serialPort->setFlowControl(this->serialConfig.vSerialFlowControl);

    this->serialPort->close();
    if(this->serialPort->open(QIODevice::ReadWrite))
    {
        isOpen = true;
        emit openSuccess();
    }
    else
    {
        isOpen = false;
        emit openFail();
    }
}

void SerialManage::disconnectSerial()
{
    this->serialPort->close();
    emit close();
}

void SerialManage::writeData(QByteArray sendBuffer)
{
    if(this->serialPort->isOpen())
    {
        this->serialPort->write(sendBuffer);
    }
    else
    {
        emit writeDataError();
    }
}

void SerialManage::readData(void)
{
    if(this->serialPort->isOpen())
    {
       QByteArray recvBuffer = this->serialPort->readAll();
       emit readFinished(recvBuffer);
    }
    else
    {
        emit readError();
    }
}

