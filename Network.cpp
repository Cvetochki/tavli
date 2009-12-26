#include "Network.h"
#include <QtGui>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

#include <iostream>

Network::Network(QWidget *parent)
    :
    m_connected(0),
    m_activeConnection(0),
    m_parent(parent),
    m_blockSize(0)
{
    m_server = new QTcpServer(this);
    m_listeningPort=1971;
    while(!m_server->listen(QHostAddress::Any,m_listeningPort)) {
        std::cerr << "Can't listen on port #" <<m_listeningPort<<std::endl;
        ++m_listeningPort;
    }
    connect(m_server,SIGNAL(newConnection()),this,SLOT(gotConnection()));
}

Network::~Network(void)
{
}

void Network::connectTo(QString str)
{
    m_client= new QTcpSocket(this);
    connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError()));
    m_client->connectToHost(str,1971);
    if (m_client->waitForConnected()) {
        connect(m_client,SIGNAL(readyRead()), this, SLOT(readNet()));
        connect(m_client,SIGNAL(disconnected()),this,SLOT(lostConnection()));
        m_connected=1;
    }
}
void Network::gotConnection(void)
{
    m_client=m_server->nextPendingConnection();
    connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError()));
    connect(m_client,SIGNAL(readyRead()), this, SLOT(readNet()));
    connect(m_client,SIGNAL(disconnected()),this,SLOT(internalLostConnection()));
    /*
	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(m_parent, tr("Tavli"),
					tr("A remote host (at %1) is trying to connect...\n"
					"Do you want to allow connection?").arg(m_client->peerAddress().toString()),
					QMessageBox::Yes | QMessageBox::No);
	if (ret != QMessageBox::Yes) {
	    disconnect(m_client, 0, 0, 0);
		m_client->disconnectFromHost();
		m_client->deleteLater();
	} else {
		emit connectedAsServer();

		m_activeConnection=1;
	}
	*/
    m_activeConnection=1;
    emit connectedAsServer(m_client->peerAddress().toString());
}

void Network::closeConnection(void)
{
    disconnect(m_client, 0, 0, 0);
    m_client->disconnectFromHost();
    m_client->deleteLater();
    m_activeConnection=0;
}
void Network::socketError()
{
    emit NetworkError(m_client->errorString());
}

void Network::netSendText(QString str)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)MSG_Text;
    out << str;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    m_client->write(block);
    m_client->flush();
}

void Network::netSendMovingPawn(int x,int y)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)MSG_MovingPawn;
    out << x;
    out << y;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    m_client->write(block);
    m_client->flush();
}

void Network::netSendGameSettings(QString name,int matchLength,int portes,int plakoto,int fevga)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)MSG_GameSettings;
    out << name;
    out << matchLength;
    out << portes;
    out << plakoto;
    out << fevga;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    m_client->write(block);
    m_client->flush();
}

void Network::readNet(void)
{
    QDataStream in(m_client);
    in.setVersion(QDataStream::Qt_4_0);
    if (m_blockSize == 0) {
        if (m_client->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> m_blockSize;
    }
    if (m_client->bytesAvailable() < m_blockSize)
        return;
    quint16 type;
    int x,y;
    QString str;
    int matchLength,portes,plakoto,fevga;
    in >> type;
    switch(type) {
    case	MSG_Text:
        in >> str;
        emit NetworkRcvMsg(str);
        break;
    case	MSG_MovingPawn:
        in >> x;
        in >> y;
        emit NetMovingPawn(x,y);
        break;
    case MSG_GameSettings:
        in >> str;
        in >> matchLength;
        in >> portes;
        in >> plakoto;
        in >> fevga;
        emit NetGameSettings(str,matchLength,portes,plakoto,fevga);
        break;
    default:
        QMessageBox::about(m_parent,"Strange network packet...","Unknown MSG_Type");
        break;
    }

    //msgDisplay->append(str);
    m_blockSize=0;
}
void Network::internalLostConnection(void)
{
    m_client->deleteLater();
    emit lostConnection();
    //
    m_activeConnection=0;
}
