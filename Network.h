#ifndef NETWORK_H
#define NETWORK_H

#include <QWidget>

class QTcpServer;
class QTcpSocket;

class Network: public QObject
{
	Q_OBJECT
public:
	int			m_listeningPort;
	int			m_connected;
	int			m_activeConnection;

	Network(QWidget *parent);
	~Network(void);

	void connectTo(QString str);
	void netSend(QString str);
private slots:
    void readNet(void);
	void socketError();
	void gotConnection();

	void lostConnection();

signals:
	void NetworkError(QString str);
	void connectedAsServer(void);
	void NetworkRcvMsg(QString str);

private:
	QWidget		*m_parent;
	QTcpServer	*m_server;
	QTcpSocket	*m_client;
	quint16		m_blockSize;
	
};

#endif
