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

QString Packet::toHex()
{
    QString format, hex;

    int size = _data.size();

    for (int i = 0; i < size; ++i)
        format = format % hex.sprintf("%02X ", (quint8)_data.at(i));

    if (_data.size() >= 1)
        format.chop(1); //Remove last space

    return format;
}
