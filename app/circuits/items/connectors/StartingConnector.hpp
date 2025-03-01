#ifndef STARTINGCONNECTOR_HPP
#define STARTINGCONNECTOR_HPP

#include "BaseConnector.hpp"

class StartingConnector : public BaseConnector
{
    Q_OBJECT
public:
    explicit StartingConnector(const StartingPoint& startPoint,
                               QPoint positionOffset = QPoint(0, 0),
                               QWidget* parent = nullptr);

    QString GetMimeType() const override;
    QPoint GetConnectionPoint() const override;
    const StartingPoint& GetStartPoint() const;
    void AddActionsToMenu(QMenu* menu) override;

public slots:
    void SetConnectionId(quint64 connId) override;
    void RemoveConnectionId(quint64 connId) override;

private:
    StartingPoint m_startPoint;
};

#endif // STARTINGCONNECTOR_HPP
