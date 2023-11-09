#include "socketmanage.h"

SocketManage::SocketManage(QObject *parent) : QObject{parent}
{
    m_server = new QTcpServer(this);
    connect(this->m_server, &QTcpServer::newConnection, this, &SocketManage::connectClient);
}

void SocketManage::setNetConfig(QString ipAddr, quint16 comPort)
{
    address = QHostAddress(ipAddr);
    port = comPort;
}

void SocketManage::connectNet()
{
    if (m_server->listen(address, port))
    {
        emit openSuccess();
    }
    else
    {
        emit openFail();
    }
}

void SocketManage::disConnectNet()
{
    m_server->close();

    for (QTcpSocket *client : clients)
    {
        client->disconnectFromHost();
    }

    clients.clear();
    emit close();
}

void SocketManage::connectClient()
{
    QTcpSocket *clientSocket = m_server->nextPendingConnection();
    clients.append(clientSocket);
    LOG << "new Client IP:" << clientSocket->peerAddress().toString() << ":" << clientSocket->peerPort();
    connect(clientSocket, &QTcpSocket::readyRead, this, &SocketManage::readData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &SocketManage::disconnectClient);
    emit newClient();
    emit updateClient(clients);
}

void SocketManage::disconnectClient()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;

    LOG << "del Client IP:" << clientSocket->peerAddress().toString() << ":" << clientSocket->peerPort();
    clients.removeOne(clientSocket);
    emit delClient();
    emit updateClient(clients);
}

void SocketManage::readData()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
        return;
    QByteArray recvMsg = clientSocket->readAll();
    emit readFinished(clientSocket, recvMsg);
}

void SocketManage::broadcastData(QByteArray buffer)
{
    for (QTcpSocket *client : clients)
    {
        client->write(buffer);
    }
}

void SocketManage::unicastData(QTcpSocket *client, QByteArray buffer)
{
    client->write(buffer);
}
