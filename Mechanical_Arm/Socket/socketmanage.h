#ifndef SOCKETMANAGE_H
#define SOCKETMANAGE_H

#include "logInfo.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class SocketManage : public QObject
{
    Q_OBJECT
  public:
    explicit SocketManage(QObject *parent = nullptr);
    void setNetConfig(QString ipAddr, quint16 comPort);
    void connectNet(void);
    void disConnectNet(void);

  public slots:

    void connectClient(void);
    void disconnectClient(void);
    void readData(void);

    void broadcastData(QByteArray buffer);
    void unicastData(QTcpSocket *client, QByteArray buffer);

  signals:
    void newClient(void);
    void delClient(void);
    void updateClient(QList<QTcpSocket *> clients);

    void openSuccess();
    void openFail();
    void close();

    void readFinished(QTcpSocket *client, QByteArray &buffer);

  private:
    QHostAddress address;
    quint16 port;

    QTcpServer *m_server;
    QList<QTcpSocket *> clients;
};
Q_DECLARE_METATYPE(const QTcpSocket *)

#endif // SOCKETMANAGE_H
