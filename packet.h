#ifndef PACKET_H
#define PACKET_H

#include <QtCore>
#include <QObject>
#include <QDataStream>

class Packet : public QObject
{
    Q_OBJECT
public:
    explicit Packet(QString type, QByteArray data, QObject* parent = 0);

    QByteArray data() { return _data; }
    QString type() { return _type; }
    ushort size() { return _size; }
    ushort opcode() { return _opcode; }

    void readHeader();
    template <typename T> T read();

    QString readString(quint16 length);
    qint8 readByte();
    qint16 readShort();
    qint32 readInt();

    QString toHex();

signals:

public slots:

private:
    QByteArray _data;
    QDataStream* _dataStream;

    QString _type;
    ushort _size;
    ushort _opcode;
};

#endif // PACKET_H
