#include "snoofle.h"

Snoofle::Snoofle(QObject *parent) : QObject(parent)
{
    _authServer = nullptr;
    _realmServer = nullptr;
}

Snoofle::~Snoofle()
{
    delete _authServer;
    delete _realmServer;
}

bool Snoofle::init()
{
    QSettings settings(qApp->applicationDirPath() + "/settings.ini", QSettings::IniFormat);

    _authServer = new ProxyServer(
                settings.value("AuthServer/listen").toUInt(),
                settings.value("AuthServer/remote").toString(),
                settings.value("RealmServer/listen").toUInt());

    QString realm = settings.value("RealmServer/realm").toString();
    _realmServer = new ProxyServer(
                settings.value("RealmServer/listen").toUInt(),
                settings.value(realm + "/ip").toString());

    if (!_authServer->listen() || !_realmServer->listen())
        return false;

    qDebug() << "[ Snoofle ]";
    qDebug() << "Wakfu sniffer by Sgt Fatality";
    qDebug() << " ";
    qDebug() << ">> AuthServer listening on port " << _authServer->localPort();
    qDebug() << ">> RealmServer listening on port " << _realmServer->localPort();
    qDebug() << " ";
    qDebug() << ">> RealmServer remote realm is " << realm;
    qDebug() << " ";
    qDebug() << ">> Waiting for client...";
    qDebug() << " ";

    return true;
}

