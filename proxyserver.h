#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include <QObject>
#include <QtNetwork>

class ProxyServer : public QObject
{
    Q_OBJECT
public:
    explicit ProxyServer(uint localPort, QString remote, QObject *parent = 0);
    ~ProxyServer();

    bool listen();

signals:

public slots:

private:
    QTcpServer* _server;
    uint _localPort;
    QHostAddress _remoteIp;
    uint _remotePort;
};

#endif // PROXYSERVER_H
