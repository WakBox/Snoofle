#include "packet.h"

Packet::Packet(QString type, QByteArray data, QObject *parent) : QObject(parent)
{
    _data    = data;
    _dataStream = new QDataStream(_data);

    _type      = type;
    _size      = 0;
    _opcode    = 0;
}

void Packet::readHeader()
{
    *_dataStream >> _size;

    if (type() == "CMSG")
    {
        qint8 unk1;
        *_dataStream >> unk1;
    }

    *_dataStream >> _opcode;
}

template <typename T> T Packet::read()
{
    T v;
    *_dataStream >> v;

    return v;
}

QString Packet::readString(quint16 length)
{
    QByteArray bytes;
    bytes.resize(length);

    for (quint16 i = 0; i < length; ++i)
        bytes[i] = readByte();

    QString string = QString(bytes);

    return string;
}

qint8 Packet::readByte() { return read<qint8>(); }
qint16 Packet::readShort() { return read<qint16>(); }
qint32 Packet::readInt() { return read<qint32>(); }
