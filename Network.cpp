#include "Network.h"
#include <QtGui>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

#include <iostream>
#ifdef USEUPNP
#include <libtorrent/upnp.hpp>
#include <libtorrent/natpmp.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#endif

Network::Network(QWidget *parent)
    :
    m_connected(0),
    m_activeConnection(0),
    m_parent(parent),
    m_blockSize(0)
{
#ifdef USEUPNP
    m_session.set_severity_level(libtorrent::alert::debug);
    m_upnp = m_session.start_upnp();
    m_natpmp = m_session.start_natpmp();
    int ret= m_upnp->add_mapping(libtorrent::upnp::tcp,1971,1971);
    int nat= m_natpmp->add_mapping(libtorrent::natpmp::tcp,1971,1971);
    std::cout << "Ret: " << ret << m_upnp->router_model() <<std::endl;
    std::cout << "Nat: " << nat <<std::endl;
    //libtorrent::alert *al;
    std::auto_ptr<libtorrent::alert> al;

    al=m_session.pop_alert();
    while (al.get()) {
        if (libtorrent::portmap_alert* p = dynamic_cast<libtorrent::portmap_alert*>(al.get()))
        {
            std::cout << p->message() <<al->message() << std::endl;
        }
        else if (libtorrent::portmap_error_alert* p = dynamic_cast<libtorrent::portmap_error_alert*>(al.get()))
        {
            std::cout << al->message() << std::endl;
        } else {
            std::cout << "Some unexpected alert: " << al->message() << std::endl;
        }
        al=m_session.pop_alert();
    }

#endif
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
#ifdef USEUPNP

    m_session.stop_natpmp();
    m_session.stop_upnp();
#endif
}

void Network::connectTo(QString str)
{
    m_client= new QTcpSocket(this);
    connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError()));
    m_client->connectToHost(str,1971);
    connect(m_client,SIGNAL(connected()),this,SLOT(slotConnectedAsClient()));
    connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError()));
    /*
    std::cout << "Gonna wait for connection..." << std::endl;
    if (m_client->waitForConnected()) {
        connect(m_client,SIGNAL(readyRead()), this, SLOT(readNet()));
        connect(m_client,SIGNAL(disconnected()),this,SLOT(lostConnection()));
        m_connected=1;
        m_activeConnection=1;
    } else
        std::cout << "Nope, no connection." << std::endl;
        */
}

void Network::slotConnectedAsClient(void)
{
    connect(m_client,SIGNAL(readyRead()), this, SLOT(readNet()));
    connect(m_client,SIGNAL(disconnected()),this,SLOT(internalLostConnection()));
    m_connected=1;
    m_activeConnection=1;
    emit connectedAsServer(m_client->peerAddress().toString());
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
