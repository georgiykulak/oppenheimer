#ifndef STARTINGCONNECTOR_HPP
#define STARTINGCONNECTOR_HPP

#include "ItemUtils.hpp"

#include <QWidget>

class BaseCircuitItem;

class StartingConnector : public QWidget
{
    Q_OBJECT
public:
    explicit StartingConnector(const StartingPoint& startPoint,
                               QPoint positionOffset = QPoint(0, 0),
                               QWidget* parent = nullptr);

    quint64 GetItemId() const;
    QPoint GetConnectorSize() { return QPoint(10, 10); }
    const StartingPoint& GetStartPoint() const;
    QPoint GetPositionOffset() const;
    void SetPixmap(const QPixmap& pixmap);
    QPixmap GetPixmap() const;
    void DrawToPixmap();

    static void DrawConnectorToPixmap(QPainter& painter, QPoint position_offset);

public slots:
    void InsertConnectionId(quint64 connId);
    void RemoveConnectionId(quint64 connId);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    BaseCircuitItem* m_sourceItem;
    QPixmap m_pixmap;
    StartingPoint m_startPoint;
    QPoint m_positionOffset;
};

#endif // STARTINGCONNECTOR_HPP
