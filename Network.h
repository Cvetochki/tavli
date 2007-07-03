#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>

class Network: public QObject
{
	Q_OBJECT
public:
	Network(void);
	~Network(void);
};

#endif
