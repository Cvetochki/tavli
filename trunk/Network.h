#ifndef NETWORK_H
#define NETWORK_H

#include <QWidget>
//#define USEUPNP
#ifdef USEUPNP
#include <libtorrent/session.hpp>


#endif

class QTcpServer;
class QTcpSocket;


#define MSG_Text		1
#define MSG_Identity		2
#define MSG_GameSettings	3
#define MSG_ReGameSettings	4
#define MSG_AcceptSettings	5
#define MSG_MovingPawn		6


class Network: public QObject
{
    Q_OBJECT
public:
    int	m_listeningPort;

    //int	m_activeConnection;

    Network(QWidget *parent);
    ~Network(void);

    void connectTo(QString str);
    void netSendText(QString str);
    void netSendMovingPawn(int x,int y);
    void netSend(int type,QByteArray str);
    void netSendGameSettings(QString name,int matchLength,int portes,int plakoto,int fevga);
    void closeConnection(void);

    bool isConnected();
signals:
    void NetworkError(QString str);
    void connectedAsServer(QString remoteHost);
    void NetworkRcvMsg(QString str);
    void NetMovingPawn(int x,int y);
    void NetGameSettings(QString str,int matchLength,int portes,int plakoto,int fevga);
    void lostConnection(void);


private slots:
    void readNet(void);
    void socketError();
    void gotConnection();
    void slotConnectedAsClient();
    void internalLostConnection();


private:
    bool        m_connected;
    QWidget	*m_parent;
    QTcpServer	*m_server;
    QTcpSocket	*m_socket;
    quint16	m_blockSize;
#ifdef USEUPNP
    libtorrent::session m_session;
    libtorrent::upnp    *m_upnp;
    libtorrent::natpmp  *m_natpmp;
#endif

};

#endif
