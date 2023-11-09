
#ifndef SERIALPROTOCOL_H
#define SERIALPROTOCOL_H

#include <QDebug>
#include <QObject>
#include <QThread>

#include "SerialManage.h"

class SerialProtocol : public QObject
{
    Q_OBJECT
  public:
    explicit SerialProtocol(QObject *parent = nullptr);
    ~SerialProtocol(void);

    void setSerialManage(SerialManage *serialmanage);

  private:
    uint8_t cheaksun(QList<uint8_t> data, uint packNum);
    void packData(uint8_t main, uint8_t sec1, uint8_t sec2, uint8_t sec3); //打包数据
    void msleep(long long msec);
    QByteArray convertQListToQByteArray(const QList<uint8_t> &list);

  public slots:
    void proData(const QByteArray &data);            //处理数据
    void controlSingleServo(uint8_t num, int angle); //控制单个舵机
    void controlMultipleServo(QList<int> data);      //控制多个舵机

  signals:
    void sendData(const QByteArray &data);
    void showData(uint8_t num, uint16_t angle);
    void finish();

  private:
    SerialManage *serialManage;
    QList<uint8_t> recvData;
};

class SerialThread : public QThread
{
    Q_OBJECT
  public:
    explicit SerialThread(QObject *parent = nullptr);

  protected:
    void run() override
    {
        qDebug() << "serial Thread runing:" << QThread::currentThread();
        exec();
    }
};

#endif // SERIALPROTOCOL_H
