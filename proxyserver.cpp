#include "proxyserver.h"
#include "worldpacket.h"

ProxyServer::ProxyServer(uint localPort, QString remote, uint proxyPort, QObject *parent) : QObject(parent)
{
    _proxy = new QTcpServer;
    _local = new QTcpSocket;
    _localPacketSize = 0;

    _remote = new QTcpSocket;
    _remotePacketSize = 0;

    _localPort = localPort;
    _remoteIp = QHostAddress(remote.split(":").first());
    _remotePort = remote.split(":").last().toUInt();
    _proxyPort = proxyPort;
}

ProxyServer::~ProxyServer()
{
    _local->deleteLater();
    _remote->deleteLater();
}

bool ProxyServer::listen()
{
    if (!_proxy->listen(QHostAddress::LocalHost, _localPort))
    {
        qDebug() << _proxy->errorString();
        return false;
    }

    connect(_proxy, SIGNAL(newConnection()), this, SLOT(onConnect()));
    connect(_remote, SIGNAL(readyRead()), this, SLOT(onRemoteRecv()));

    return true;
}

void ProxyServer::onConnect()
{
    _local = _proxy->nextPendingConnection();

    if (_local)
    {
        connect(_local, SIGNAL(readyRead()), this, SLOT(onLocalRecv()));
        _remote->connectToHost(_remoteIp, _remotePort);

        qDebug() << ">> Connecting to remote: " << _remoteIp << ":" << _remotePort;

        if (_remote->waitForConnected(2000))
        {
            qDebug() << ">> Client successfully connected to Snoofle!";
            qDebug() << ">> Snoofle is sniffing now..";
            return;
        }
    }

    qDebug() << "Error while connecting local socket to remote socket " << _remoteIp;
}

void ProxyServer::onLocalRecv()
{
    QDataStream in(_local);

    while (_local->bytesAvailable())
    {
        if (_localPacketSize == 0)
        {
            if (_local->bytesAvailable() < (qint64)sizeof(quint16))
                return;

            in >> _localPacketSize;
        }

        if ((_local->bytesAvailable() + 2) < _localPacketSize)
            return;

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream << _localPacketSize;
        data += in.device()->read((qint64)(_localPacketSize - sizeof(quint16)));

        Packet* packet = new Packet("CMSG", data);
        packet->readHeader();

        qDebug() << ">> localRecv " << packet->opcode();

        _remote->write(packet->data());
        _localPacketSize = 0;

        emit packetReceived(packet);
    }
}

void ProxyServer::onRemoteRecv()
{
    QDataStream in(_remote);

    while (_remote->bytesAvailable())
    {
        if (_remotePacketSize == 0)
        {
            if (_remote->bytesAvailable() < (qint64)sizeof(quint16))
                return;

            in >> _remotePacketSize;
        }

        if ((_remote->bytesAvailable() + 2) < _remotePacketSize)
            return;

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);

        stream << _remotePacketSize;
        data += in.device()->read((qint64)(_remotePacketSize - sizeof(quint16)));

        Packet* packet = new Packet("SMSG", data);
        packet->readHeader();

        qDebug() << ">> onRemoteRecv " << packet->opcode();

        if (_proxyPort && packet->opcode() == REALM_LIST_PACKET)
            _local->write(updateRealms(packet));
        else
            _local->write(packet->data());

        _remotePacketSize = 0;

        if (_proxyPort && (
                packet->opcode() == AUTH_ENCRYPTED_LOGIN || packet->opcode() == AUTH_ACCOUNT_DETAILS)
                )
            return;


        emit packetReceived(packet);
    }
}

QByteArray ProxyServer::updateRealms(Packet* packet)
{
    QMap<int, Realm> realms;

    int count = packet->readInt();

    for (quint8 i = 0; i < count; ++i)
    {
        Realm realm;

        realm.id = packet->readInt();
        realm.name = packet->readString((quint16)packet->readInt());

        realm.community = packet->readInt();
        realm.ip = packet->readString((qint16)packet->readInt());

        int ports = packet->readInt();
        for (quint8 j = 0; j < ports; ++j)
            realm.ports.push_back(packet->readInt());

        realm.order = packet->readByte();

        realms[realm.id] = realm;
    }

    count = packet->readInt();
    for (quint8 i = 0; i < count; ++i)
    {
        int serverId = packet->readInt();
        Realm& realm = realms[serverId];

        realm.versionSize = packet->readInt();
        {
            realm.versionP1 = packet->readByte();
            realm.versionP2 = packet->readShort();
            realm.versionP3 = packet->readByte();
            realm.build = packet->readString((qint16)packet->readByte());
        }

        realm.configSize = packet->readInt();
        {
            int configSize = packet->readInt();

            for (int j = 0; j < configSize; ++j)
            {
                Config c;
                c.key = packet->readShort();
                c.value = packet->readString(packet->readInt());

                realm.configs.push_back(c);
            }
        }

        realm.locked = packet->readByte();
    }

    for (QMap<int, Realm>::Iterator itr = realms.begin(); itr != realms.end(); ++itr)
    {
        (*itr).name += " Snoofle";
        (*itr).ip = "127.0.0.1";
        (*itr).ports.clear();
        (*itr).ports.push_back(_proxyPort);
        (*itr).ports.push_back(443);
    }

    WorldPacket data(REALM_LIST_PACKET);

    data << int(realms.count());

    for (QMap<int, Realm>::ConstIterator itr = realms.begin(); itr != realms.end(); ++itr)
    {
        Realm r = (*itr);

        if (r.id == 0)
            continue;

        data << int(r.id);

        data << int(r.name.length());
        data.WriteRawBytes(r.name.toLatin1().constData(), (int)r.name.length());

        data << int(r.community);
        data << int(r.ip.length());
        data.WriteRawBytes(r.ip.toLatin1().constData(), (int)r.ip.length());

        data << int(r.ports.size());
        for (QList<int>::ConstIterator it = r.ports.begin(); it != r.ports.end(); ++it)
            data << int((*it));

        data << qint8(r.order);
    }

    data << int(realms.size());

    for (QMap<int, Realm>::ConstIterator itr = realms.begin(); itr != realms.end(); ++itr)
    {
        Realm r = (*itr);

        if (r.id == 0)
            continue;

        data << int(r.id);

        data << int(r.versionSize);
        data << qint8(r.versionP1);
        data << (qint16)r.versionP2;
        data << (qint8)r.versionP3;

        data << quint8(r.build.length());
        data.WriteRawBytes(r.build.toLatin1().constData(), (int)r.build.length());

        data << (int)r.configSize;
        data << (int)r.configs.size();

        for (QList<Config>::ConstIterator it = r.configs.begin(); it != r.configs.end(); ++it)
        {
            data << (*it).key;

            QString value = (*it).value;
            data << int(value.length());
            data.WriteRawBytes(value.toLatin1().constData(), (int)value.length());
        }

        data << (qint8)r.locked;
    }

    data.WriteHeader();

    qDebug() << ">> Realms updated to use Snoofle.";

    return data.GetPacket();
}
