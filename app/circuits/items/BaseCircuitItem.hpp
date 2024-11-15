#ifndef BASECIRCUITITEM_HPP
#define BASECIRCUITITEM_HPP

#include "ItemUtils.hpp"

#include <vector>

#include <QWidget>

class EndingConnector;
class StartingConnector;

class BaseCircuitItem : public QWidget
{
    Q_OBJECT
public:
    explicit BaseCircuitItem(QWidget *parent = nullptr);

    virtual inline ItemType GetItemType() const noexcept
    { WarnNotImplemented(ItemType(-1)); }

    void SetId(quint64 id);
    quint64 GetId() const;
    void SetSize(QSize size);
    QSize GetSize() const;

    virtual void DrawToPixmap()              { WarnNotImplemented(); }
    virtual void SetOrderId(int)             { WarnNotImplemented(); }
    virtual void SetValue(bool)              { WarnNotImplemented(); }

    virtual std::vector<EndingConnector*> GetEndingConnectors() const;
    virtual std::vector<StartingConnector*> GetStartingConnectors() const;

    virtual void RemoveConnectionId(quint64) { WarnNotImplemented(); }

signals:
    virtual bool closeDialogs();

protected:
    quint64 m_id = 0;
    QSize m_size;
    std::vector<EndingConnector*> m_endingConnectors;
    std::vector<StartingConnector*> m_startingConnectors;
};

#endif // BASECIRCUITITEM_HPP
