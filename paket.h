#ifndef PAKET_H
#define PAKET_H

#include <QDateTime>
#include <QColor>

class packet_t
{
public:
    packet_t():
        dest_addr(0),
        source_addr(0),
        type(0),
        address(0),
        data(0),
        checksum(0),
        checksumLocal(0),
        color(random())
    {

    }

    quint8 dest_addr;
    quint8 source_addr;
    quint8 type;
    quint16 address;
    quint32 data;
    quint8 checksum;
    quint8 checksumLocal;
    QDateTime time;
    QString name;
    QColor color;
};


#define TYPES "IDLE" << "GET" << "SET" << "OKSET" << "OKGET" << "NOKSET" << "NOKGET" << "IDLEOK" << "ERROR";

#endif // PAKET_H
