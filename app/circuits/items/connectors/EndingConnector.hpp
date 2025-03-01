#ifndef ENDINGCONNECTOR_HPP
#define ENDINGCONNECTOR_HPP

#include "BaseConnector.hpp"

class EndingConnector : public BaseConnector
{
    Q_OBJECT
public:
    explicit EndingConnector(const EndingPoint& endPoint,
                             QPoint positionOffset = QPoint(0, 0),
                             QWidget* parent = nullptr);

    QString GetMimeType() const override;
    QPoint GetConnectionPoint() const override;
    const EndingPoint& GetEndPoint() const;
    std::size_t GetConnectionId() const;
    bool IsConnected() const;
    void AddActionsToMenu(QMenu* menu) override;

public slots:
    void SetConnectionId(quint64 connId) override;
    void RemoveConnectionId(quint64 connId) override;

private:
    EndingPoint m_endPoint;
};

#endif // ENDINGCONNECTOR_HPP
