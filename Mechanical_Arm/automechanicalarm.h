
#ifndef AUTOMECHANICALARM_H
#define AUTOMECHANICALARM_H

#include <QDebug>
#include <QEventLoop>
#include <QMutex>
#include <QObject>
#include <QPointF>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>

#include "Serial/serialprotocol.h"
#include "Socket/socketprotocol.h"
#include "imageproc.h"

class autoMechanicalArm : public QObject
{
    Q_OBJECT
  public:
    explicit autoMechanicalArm(QObject *parent = nullptr);
    ~autoMechanicalArm();

    void setSerialProtocol(SerialProtocol *serialProtocol);
    void setSocketProtocol(SocketProtocol *socketProtocol);

    void setWaitCondition(QWaitCondition *condition)
    {
        this->WaitCondition = condition;
    }

    void setMutex(QMutex *mutex)
    {
        this->mutex = mutex;
    }

  public slots:
    void start(void);
    void stop(void);
    void setCycleMode(bool mode);
    void recvQImage(QImage image);

  private slots:

    void recvThingsPlaceData(QMultiMap<QString, QPointF> thingsPlaceData);
    void execute();

  private:
    void taskList();

    void openClip(void);
    void closeClip(void);
    void workMode(void);
    void placeMode(void);

    void sendServoQListData(QList<int> data);
    void msleep(long msec);

  signals:
    void mFinish(void);
    void mGetImage(void);
    void mNotThing(QString str);
    void mSendServoData(uint8_t num, int angle);
    void mSendServoQListData(QList<int> data);
    void calculateComplete(void);

    void returnResultsImage(QImage image);

  private:
    QString color;
    QList<int> AnglePose;

    QWaitCondition *WaitCondition = nullptr;
    QMutex *mutex = nullptr;

    SerialProtocol *serialProtocol = nullptr;
    SocketProtocol *socketProtocol = nullptr;

  private:
    imageProcessor imageProc;

  private:
    int taskMark = 0;
    bool finishFlag = false;
    bool cycleMode = false;
    bool taskRunFlag = false;
};

class autoMechanicalArmThread : public QThread
{
    Q_OBJECT

  public:
    explicit autoMechanicalArmThread(QObject *parent = nullptr);

  protected:
    void run(void) override
    {
        qDebug() << "task Thread runing:" << QThread::currentThread();
        exec();
    }

  signals:
};

#endif // AUTOMECHANICALARM_H
