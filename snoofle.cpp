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
                settings.value("AuthServer/remote").toString());

    _realmServer = new ProxyServer(
                settings.value("RealmServer/listen").toUInt(),
                settings.value(settings.value("RealmServer/realm").toString() + "/ip").toString());

    if (!_authServer->listen() || !_realmServer->listen())
        return false;

    return true;
}

