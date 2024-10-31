#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <QObject>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);

    Connection(const Connection& c) {m_startId = c.m_startId; m_endId = c.m_endId;}

    quint64 m_startId;
    quint64 m_endId;
};

#endif // CONNECTION_HPP
