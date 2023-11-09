#ifndef SOCKETPROTOCOL_H
#define SOCKETPROTOCOL_H

#include <QObject>
#include <QThread>

#include "Socket/socketmanage.h"
#include "logInfo.h"

class SocketProtocol : public QObject
{
    Q_OBJECT
  public:
    explicit SocketProtocol(QObject *parent = nullptr);
    void setSocketManage(SocketManage *socketmanage);

  public slots:
    void proData(QTcpSocket *client, const QByteArray &buffer);

    void controlSingleServo(uint8_t num, int angle); // 控制单个舵机
    void controlMultipleServo(QList<int> data);      // 控制多个舵机

  signals:
    void sendData(const QByteArray &data);
    void showData(uint8_t num, uint16_t angle);
    void finish();

  private:
    uint8_t cheaksun(QList<uint8_t> data, int packNum);
    void packData(uint8_t main, uint8_t sec1, uint8_t sec2, uint8_t sec3);                     // 打包数据
    void packData(QTcpSocket *client, uint8_t main, uint8_t sec1, uint8_t sec2, uint8_t sec3); // 打包数据
    void msleep(long long msec);
    QByteArray convertQListToQByteArray(const QList<uint8_t> &list);

  private:
    SocketManage *m_socketManage;
    QList<uint8_t> recvData;
};

class SocketThread : public QThread
{
    Q_OBJECT
  public:
    explicit SocketThread(QObject *parent = nullptr);

  protected:
    void run() override
    {
        qDebug() << "TCP线程运行,地址:" << QThread::currentThread();
        exec();
    }
};

#endif // SOCKETPROTOCOL_H
