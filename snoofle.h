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

private:
    ProxyServer* _authServer;
    ProxyServer* _realmServer;
};

#endif // SNOOFLE_H
