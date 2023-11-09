#include "register.h"
#include <QCameraInfo>
#include <QMetaType>
#include <QTcpSocket>
void RegisterAll()
{
    qRegisterMetaType<QCameraInfo>("QCameraInfo");
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<QByteArray>("QByteArray&");
    qRegisterMetaType<QList<QTcpSocket *>>("QList<QTcpSocket*>");
    qRegisterMetaType<QList<int>>("QList<int>");
    qRegisterMetaType<QImage>("const QImage");
}
