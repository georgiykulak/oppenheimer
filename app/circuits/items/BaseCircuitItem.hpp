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
    explicit BaseCircuitItem(QWidget* parent = nullptr);
    virtual ~BaseCircuitItem();

    virtual inline ItemType GetItemType() const noexcept
    { WarnNotImplemented(ItemType(Invalid)); }
    quint64 GetId() const;

    virtual void DrawToPixmap()              { WarnNotImplemented(); }
    virtual void SetValue(bool)              { WarnNotImplemented(); }

    virtual std::vector<EndingConnector*> GetEndingConnectors() const;
    virtual std::vector<StartingConnector*> GetStartingConnectors() const;

    virtual void RemoveConnectionId(quint64) { WarnNotImplemented(); }

signals:
    bool closeDialogs();

public slots:
    void SetOrderId(int orderId);
    void SetColor(const QColor& color);

protected:
    std::vector<EndingConnector*> m_endingConnectors;
    std::vector<StartingConnector*> m_startingConnectors;
    QPixmap m_pixmap;
    QColor m_color = Qt::white;
    quint64 m_id = 0;
    int m_orderId = -1;

    virtual void paintEvent(QPaintEvent* event) override;
};

#endif // BASECIRCUITITEM_HPP
