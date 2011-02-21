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
    m_connected(false),
    //m_activeConnection(0),
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

bool Network::isConnected(void)
{
    return m_connected;
}

void Network::connectTo(QString str)
{
    m_socket= new QTcpSocket(this);
    m_socket->connectToHost(str,1971);
    connect(m_socket,SIGNAL(connected()),this,SLOT(slotConnectedAsClient()));
    connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError()));
}

void Network::slotConnectedAsClient(void)
{
    connect(m_socket,SIGNAL(readyRead()), this, SLOT(readNet()));
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(internalLostConnection()));
    connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError()));
    m_connected=true;
    emit connected(m_socket->peerAddress().toString());
}

void Network::gotConnection(void)
{
    m_socket=m_server->nextPendingConnection();
    slotConnectedAsClient();
}

void Network::closeConnection(void)
{
    disconnect(m_socket, 0, 0, 0);
    m_socket->disconnectFromHost();
    m_socket->deleteLater();
    m_connected=false;
}
void Network::socketError()
{
    m_connected=false;
    emit NetworkError(m_socket->errorString());
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
    m_socket->write(block);
    m_socket->flush();
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
    m_socket->write(block);
    m_socket->flush();
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
    m_socket->write(block);
    m_socket->flush();
}

void Network::readNet(void)
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_4_0);
    if (m_blockSize == 0) {
        if (m_socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> m_blockSize;
    }
    if (m_socket->bytesAvailable() < m_blockSize)
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
    m_socket->deleteLater();
    emit lostConnection();
    //
    m_connected=false;
}
