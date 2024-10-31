#ifndef CIRCUITINPUT_HPP
#define CIRCUITINPUT_HPP

#include "BaseCircuitItem.hpp"

class StartingConnector;

class CircuitInput : public BaseCircuitItem
{
    Q_OBJECT
public:
    explicit CircuitInput(const StartingPoint& startPoint, QWidget *parent = nullptr);

    inline ItemType GetItemType() const noexcept final
    { return ItemType::Input; }

    virtual void SetPixmap(const QPixmap& pixmap) override;
    virtual QPixmap GetPixmap() const override;
    virtual void DrawToPixmap() override;

    void SetOrderId(int orderId) final;
    int GetOrderId() const final;
    void SetValue(bool value) final;
    bool GetValue() const final;

    void SetColor(const QColor& color);
    QColor GetColor() const;

    const StartingPoint& GetStartPoint() const;
    void RemoveConnectionId(quint64 connId);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_pixmap;
    StartingConnector* m_startingConnector;

    quint64 m_id = 0;
    int m_orderId = -1;
    bool m_inputValue = 0;

    QColor m_color = Qt::gray;
};

#endif // CIRCUITINPUT_HPP
