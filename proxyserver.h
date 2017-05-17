#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include <QObject>
#include <QtNetwork>

#include "packet.h"

#define AUTH_ENCRYPTED_LOGIN 1026
#define AUTH_ACCOUNT_DETAILS 1027
#define REALM_LIST_PACKET 1036

struct Config
{
    short key;
    QString value;
};

struct Realm
{
    int id;
    QString name;
    int community;
    QString ip;
    QList<int> ports;
    qint8 order;

    int versionSize;
    qint8 versionP1;
    qint16 versionP2;
    qint8 versionP3;
    QString build;

    int configSize;
    QList<Config> configs;

    bool locked;
};

class ProxyServer : public QObject
{
    Q_OBJECT
public:
    explicit ProxyServer(uint localPort, QString remote, uint proxyPort = 0, QObject *parent = 0);
    ~ProxyServer();

    bool listen();
    QByteArray updateRealms(Packet* packet);

    uint localPort() { return _localPort; }

signals:
    void packetReceived(Packet* packet);

public slots:
    void onConnect();
    void onLocalRecv();
    void onRemoteRecv();

private:
    QTcpServer* _proxy;

    QTcpSocket* _local;
    quint16 _localPacketSize;

    QTcpSocket* _remote;
    quint16 _remotePacketSize;

    uint _localPort;
    QHostAddress _remoteIp;
    uint _remotePort;
    uint _proxyPort;
};

#endif // PROXYSERVER_H
