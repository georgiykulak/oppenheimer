#ifndef ITEMUTILS_H
#define ITEMUTILS_H

#include <QDebug>
#include <QPoint>

#include <set>

#define WarnNotImplemented(val) \
    qWarning() << __PRETTY_FUNCTION__ << ": not implemented"; return val;

enum ItemType
{
    Invalid = 0,
    Input,
    Output,
    Element
};

struct EndingPoint
{
    QPoint connPos;
    quint64 connId = 0;
};

using EndingPointVector = std::vector<EndingPoint>;

struct StartingPoint
{
    using IdsSet = std::set<quint64>;

    QPoint connPos;
    IdsSet connIds;
};

using StartingPointVector = std::vector<StartingPoint>;

const QString inputMime = "application/x-oph-dndcircuitinput";
const QString outputMime = "application/x-oph-dndcircuitoutput";
const QString elementMime = "application/x-oph-dndcircuitelement";
const QString endingConnectorMime = "application/x-oph-endingconnector";
const QString startingConnectorMime = "application/x-oph-startingconnector";

#endif // ITEMUTILS_H
