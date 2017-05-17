#include "snoofle.h"

Snoofle::Snoofle(QObject *parent) : QObject(parent)
{
    _authServer = nullptr;
    _realmServer = nullptr;
    _dump = nullptr;

    qDebug() << "[ Snoofle ]";
    qDebug() << "Wakfu sniffer by Sgt Fatality";
    qDebug() << " ";
}

Snoofle::~Snoofle()
{
    qDebug() << "Exiting Snoofle...";

    delete _authServer;
    delete _realmServer;

    if (_dump)
    {
        _dump->close();
        _dump->deleteLater();
    }
}

bool Snoofle::init()
{
    _dump = new QFile(qApp->applicationDirPath() +
                      "/snoofle_dump_" +
                      QDateTime::currentDateTime().toString("dd-MM-yyyy_hh-mm") +
                      ".wxy", this);

    if (!_dump->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << ">> Error while opening dump file: " << _dump->errorString();
        return false;
    }

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

    connect(_authServer, SIGNAL(packetReceived(Packet*)), this, SLOT(dumpPacket(Packet*)));
    connect(_realmServer, SIGNAL(packetReceived(Packet*)), this, SLOT(dumpPacket(Packet*)));

    qDebug() << ">> AuthServer listening on port " << _authServer->localPort();
    qDebug() << ">> RealmServer listening on port " << _realmServer->localPort();
    qDebug() << " ";
    qDebug() << ">> RealmServer remote realm is " << realm;
    qDebug() << " ";
    qDebug() << ">> Waiting for client...";
    qDebug() << " ";

    return true;
}

void Snoofle::dumpPacket(Packet* packet)
{
    QString out = packet->type() + ";" + packet->toHex();
    _dump->write(out.toLatin1() + "\n");
    _dump->flush();
}
