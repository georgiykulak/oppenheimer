#ifndef CIRCUITOUTPUT_HPP
#define CIRCUITOUTPUT_HPP

#include "BaseCircuitItem.hpp"

class EndingConnector;

class CircuitOutput : public BaseCircuitItem
{
    Q_OBJECT
public:
    explicit CircuitOutput(const EndingPoint& endPoint, QWidget *parent = nullptr);

    inline ItemType GetItemType() const noexcept final
    { return ItemType::Output; }

    virtual void SetPixmap(const QPixmap& pixmap) override;
    virtual QPixmap GetPixmap() const override;
    virtual void DrawToPixmap() override;

    void SetOrderId(int orderId) final;
    int GetOrderId() const final;
    void SetValue(bool value) final;
    bool GetValue() const final;

    void SetColor(const QColor& color);
    QColor GetColor() const;

    const EndingPoint& GetEndPoint() const;
    void RemoveConnectionId(quint64 connId);

signals:
    bool closeDialogs() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_pixmap;
    EndingConnector* m_endingConnector;

    quint64 m_id = 0;
    int m_orderId = -1;
    bool m_outputValue = 0;

    QColor m_color = Qt::darkGray;
};

#endif // CIRCUITOUTPUT_HPP
