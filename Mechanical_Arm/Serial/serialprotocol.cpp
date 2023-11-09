#include "serialprotocol.h"
#include <QDataStream>
#include <QEventLoop>
#include <QTimer>

SerialProtocol::SerialProtocol(QObject *parent) : QObject{parent}
{
}

SerialProtocol::~SerialProtocol()
{
    this->serialManage->deleteLater();
}

void SerialProtocol::setSerialManage(SerialManage *serialmanage)
{
    this->serialManage = serialmanage;
    connect(this->serialManage, &SerialManage::readFinished, this, &SerialProtocol::proData);
}

uint8_t SerialProtocol::cheaksun(QList<uint8_t> data, uint packNum)
{
    uint16_t temp = data[0 + packNum * 8] + data[1 + packNum * 8] + data[2 + packNum * 8] + data[3 + packNum * 8] +
                    data[4 + packNum * 8] + data[5 + packNum * 8] + data[7 + packNum * 8];
    uint8_t value = (temp % 0xFF) & 0xFF;
    return value;
}

void SerialProtocol::packData(uint8_t main, uint8_t sec1, uint8_t sec2, uint8_t sec3)
{
    QList<uint8_t> List_data = {0x55, 0x0A, main, sec1, sec2, sec3, 0x00, 0xBB};

    List_data.replace(6, cheaksun(List_data, 0));

    QByteArray byteArray = convertQListToQByteArray(List_data);

    this->serialManage->writeData(byteArray);
}

void SerialProtocol::msleep(long long msec)
{
    if (msec <= 0)
    {
        return;
    }
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, &QEventLoop::quit);
    loop.exec();
}

QByteArray SerialProtocol::convertQListToQByteArray(const QList<uint8_t> &list)
{
    QByteArray byteArray;
    byteArray.resize(list.size());

    for (int i = 0; i < list.size(); ++i)
    {
        byteArray[i] = static_cast<char>(list[i]);
    }

    return byteArray;
}

void SerialProtocol::proData(const QByteArray &data)
{
    uint16_t angle = 0;
    recvData.clear();

    for (int i = 0; i < data.size(); i++)
    {
        recvData.append(static_cast<uint8_t>(data[i]));
    }

    if (recvData.size() != 8) // 发生粘包
    {
        int packNum = (recvData.size() / 8);
        LOG << "发生粘包"
            << "包数量:" << packNum;
        for (int i = 0; i < packNum; i++)
        {
            if (recvData[0 + 8 * i] == 0x55 && recvData[1 + 8 * i] == 0x0B && recvData[7 + 8 * i] == 0xBB)
            {
                if (cheaksun(recvData, i) == recvData[6 + 8 * i])
                {
                    if (recvData[2 + 8 * i] == 0x00)
                    {
                        angle = ((recvData[4 + 8 * i] << 8) | recvData[5 + 8 * i]);
                        emit showData(recvData[3], angle);
                    }
                    else if (recvData[2 + 8 * i] == 0x01)
                    {
                        emit finish();
                    }
                }
            }
        }
        return;
    }

    if (recvData[0] == 0x55 && recvData[1] == 0x0B && recvData[7] == 0xBB)
    {
        if (cheaksun(recvData, 0) == recvData[6])
        {
            if (recvData[2] == 0x00)
            {
                angle = ((recvData[4] << 8) | recvData[5]);
                emit showData(recvData[3], angle);
            }
            else if (recvData[2] == 0x01)
            {
                emit finish();
            }
        }
    }
}

void SerialProtocol::controlSingleServo(uint8_t num, int angle)
{
    if (angle > 270)
    {
        angle = 270;
    }
    else if (angle < 0)
    {
        angle = 0;
    }
    uint8_t H_data = (angle & 0xFF00) >> 8;
    uint8_t L_data = angle & 0x00FF;
    packData(0x01, num, H_data, L_data);
}

void SerialProtocol::controlMultipleServo(QList<int> data)
{
    uint8_t H_data, L_data;

    for (int var = 0; var < data.size(); var++)
    {
        H_data = (data.at(var) & 0xFF00) >> 8;
        L_data = data.at(var) & 0x00FF;
        packData(0x00, var, H_data, L_data);
        this->msleep(20);
    }
    packData(0x10, 0, 0, 0);
}

SerialThread::SerialThread(QObject *parent) : QThread(parent)
{
}
