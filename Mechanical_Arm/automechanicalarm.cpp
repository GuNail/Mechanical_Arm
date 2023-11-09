#include "automechanicalarm.h"
#include "calculateangles.h"

autoMechanicalArm::autoMechanicalArm(QObject *parent) : QObject(parent)
{
    connect(this, &autoMechanicalArm::calculateComplete, this, &autoMechanicalArm::execute);
    connect(&imageProc, &imageProcessor::sendThingsPlace, this, &autoMechanicalArm::recvThingsPlaceData);
    connect(&imageProc, &imageProcessor::sendQimage, this, [&](QImage Image) { emit returnResultsImage(Image); });
}

autoMechanicalArm::~autoMechanicalArm()
{
    serialProtocol->deleteLater();
}

void autoMechanicalArm::setSerialProtocol(SerialProtocol *serialProtocol)
{
    this->serialProtocol = serialProtocol;
    connect(this, &autoMechanicalArm::mSendServoData, this->serialProtocol, &SerialProtocol::controlSingleServo);
    connect(this, &autoMechanicalArm::mSendServoQListData, this->serialProtocol, &SerialProtocol::controlMultipleServo);
    if (this->socketProtocol != nullptr)
    {
        disconnect(this, &autoMechanicalArm::mSendServoData, this->socketProtocol, &SocketProtocol::controlSingleServo);
        disconnect(this, &autoMechanicalArm::mSendServoQListData, this->socketProtocol,
                   &SocketProtocol::controlMultipleServo);
    }
}

void autoMechanicalArm::setSocketProtocol(SocketProtocol *socketProtocol)
{
    this->socketProtocol = socketProtocol;

    connect(this, &autoMechanicalArm::mSendServoData, this->socketProtocol, &SocketProtocol::controlSingleServo);
    connect(this, &autoMechanicalArm::mSendServoQListData, this->socketProtocol, &SocketProtocol::controlMultipleServo);

    if (this->serialProtocol != nullptr)
    {
        disconnect(this, &autoMechanicalArm::mSendServoData, this->serialProtocol, &SerialProtocol::controlSingleServo);
        disconnect(this, &autoMechanicalArm::mSendServoQListData, this->serialProtocol,
                   &SerialProtocol::controlMultipleServo);
    }
}

void autoMechanicalArm::start()
{
    emit mGetImage();
}

void autoMechanicalArm::stop()
{
    this->taskRunFlag = false;
}

void autoMechanicalArm::setCycleMode(bool mode)
{
    this->cycleMode = mode;
}

void autoMechanicalArm::recvQImage(QImage image)
{
    imageProc.getThingsPosition(image);
}

void autoMechanicalArm::recvThingsPlaceData(QMultiMap<QString, QPointF> thingsPlaceData)
{
    if (!thingsPlaceData.empty())
    {
        int X, Y, Z = 0;

        QPointF nearestPoint;
        QString nearestPointColor;

        qreal minDistance = std::numeric_limits<qreal>::max();

        // 输出 QMultiMap 中的键值对
        for (const QString &key : thingsPlaceData.uniqueKeys())
        {
            QList<QPointF> values = thingsPlaceData.values(key);

            // LOG << key << ":" << values;

            for (const QPointF &value : values)
            {
                qreal distance = value.manhattanLength();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestPoint = value;
                    nearestPointColor = key;
                }
            }
        }

        //        if(fabs(nearestPoint.x())>10||fabs(nearestPoint.y())>10)
        //        {
        //            Z=2;
        //        }

        X = qRound(nearestPoint.x());
        Y = qRound(nearestPoint.y());

        AnglePose.clear();
        color.clear();

        color = nearestPointColor;
        AnglePose = Inverse_Kinematics(X, Y, Z);

        qDebug() << "X" << X << "Y" << Y << "Z" << Z << " " << color << " " << AnglePose;

        this->taskMark = 0;
        taskRunFlag = true;
        emit calculateComplete();
    }
    else
    {
        stop();
        emit mNotThing("没有识别到物品!");
        emit mFinish();
    }
}

void autoMechanicalArm::execute()
{
    while (this->taskRunFlag)
    {
        taskList();
        this->mutex->lock();
        this->WaitCondition->wait(this->mutex);
        this->mutex->unlock();
        this->msleep(300);
    }

    if (this->finishFlag)
    {
        this->finishFlag = false;
        if (this->cycleMode)
        {
            LOG << "连续抓起\n";
            this->taskRunFlag = true;
            emit mGetImage();
        }
        else
        {
            LOG << "完成，退出\n";
            emit mFinish();
        }
    }
    else
    {
        LOG << "用户退出\n";
        emit mFinish();
    }
}

void autoMechanicalArm::taskList()
{
    switch (this->taskMark)
    {
        case 0:
            openClip();
            this->taskMark = 1;
            break;

        case 1:
            workMode();
            this->taskMark = 2;
            break;

        case 2:
            sendServoQListData(this->AnglePose);
            this->taskMark = 3;
            break;

        case 3:
            closeClip();
            this->taskMark = 5;
            break;

        case 4:
            workMode();
            this->taskMark = 5;
            break;

        case 5:
            placeMode();
            this->taskMark = 6;
            break;

        case 6:
            openClip();
            this->taskMark = 7;
            break;

        case 7:
            workMode();
            this->taskMark = 0;
            this->finishFlag = true;
            this->taskRunFlag = false;
            break;
    }
}

void autoMechanicalArm::openClip()
{
    emit mSendServoData(5, 40);
}

void autoMechanicalArm::closeClip()
{
    emit mSendServoData(5, 90);
}

void autoMechanicalArm::workMode()
{
    QList<int> data = {90, 30, 180, 160};
    emit mSendServoQListData(data);
}

void autoMechanicalArm::placeMode()
{
    QList<int> data;
    if (color == "red")
    {
        data = {158, 112, 172, 110};
    }
    else if (color == "blue")
    {
        data = {190, 90, 180, 126};
    }
    else if (color == "yellow")
    {
        data = {144, 152, 90, 142};
    }
    emit mSendServoQListData(data);
}

void autoMechanicalArm::sendServoQListData(QList<int> data)
{
    emit mSendServoQListData(data);
}

void autoMechanicalArm::msleep(long msec)
{
    if (msec <= 0)
    {
        return;
    }
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, &QEventLoop::quit);
    loop.exec();
}

autoMechanicalArmThread::autoMechanicalArmThread(QObject *parent) : QThread(parent)
{
}
