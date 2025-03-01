#ifndef BASECONNECTOR_HPP
#define BASECONNECTOR_HPP

#include "ItemUtils.hpp"

#include <QWidget>

class BaseCircuitItem;

class BaseConnector : public QWidget
{
    Q_OBJECT
public:
    BaseConnector(QWidget* parent = nullptr);

    virtual QString GetMimeType() const
    { WarnNotImplemented("text/plain"); }
    virtual QPoint GetConnectionPoint() const
    { WarnNotImplemented({}); }

    QPoint GetConnectorSize() { return QPoint(10, 10); }
    quint64 GetItemId() const;
    QPoint GetPositionOffset() const;
    virtual void AddActionsToMenu(QMenu*) {}

    QPixmap GetPixmap() const;
    void DrawToPixmap();

    static void DrawConnectorToPixmap(QPainter& painter, QPoint position_offset);

signals:
    void removeConnectionById(quint64 connId);

public slots:
    virtual void SetConnectionId(quint64) {};
    virtual void RemoveConnectionId(quint64) {};

protected:
    void paintEvent(QPaintEvent *event) override;

    BaseCircuitItem* m_sourceItem;
    QPixmap m_pixmap;
    QPoint m_positionOffset;
};

#endif // BASECONNECTOR_HPP
