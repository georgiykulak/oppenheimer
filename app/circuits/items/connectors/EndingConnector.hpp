#ifndef ENDINGCONNECTOR_HPP
#define ENDINGCONNECTOR_HPP

#include "ItemUtils.hpp"

#include <QWidget>

class BaseCircuitItem;

class EndingConnector : public QWidget
{
    Q_OBJECT
public:
    explicit EndingConnector(const EndingPoint& endPoint,
                             QPoint positionOffset = QPoint(0, 0),
                             QWidget* parent = nullptr);

    quint64 GetItemId() const;
    QPoint GetConnectorSize() { return QPoint(10, 10); }
    const EndingPoint& GetEndPoint() const;
    QPoint GetPositionOffset() const;
    std::size_t GetConnectionId() const;
    bool IsConnected() const;
    void SetPixmap(const QPixmap& pixmap);
    QPixmap GetPixmap() const;
    void DrawToPixmap();

    static void DrawConnectorToPixmap(QPainter& painter, QPoint position_offset);

public slots:
    void SetConnectionId(quint64 connId);
    void RemoveConnectionId(quint64 connId);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    BaseCircuitItem* m_sourceItem;
    QPixmap m_pixmap;
    EndingPoint m_endPoint;
    QPoint m_positionOffset;
};

#endif // ENDINGCONNECTOR_HPP
