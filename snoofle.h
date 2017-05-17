#ifndef SNOOFLE_H
#define SNOOFLE_H

#include <QObject>
#include <QtNetwork>

#include "proxyserver.h"

class Snoofle : public QObject
{
    Q_OBJECT
public:
    explicit Snoofle(QObject *parent = 0);
    ~Snoofle();

    bool init();

signals:

public slots:
    void dumpPacket(Packet* packet);

private:
    ProxyServer* _authServer;
    ProxyServer* _realmServer;

    QFile* _dump;
};

#endif // SNOOFLE_H
