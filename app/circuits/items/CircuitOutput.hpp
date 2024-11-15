#ifndef CIRCUITOUTPUT_HPP
#define CIRCUITOUTPUT_HPP

#include "BaseCircuitItem.hpp"
#include "mime/CircuitOutputMimeData.hpp"

class EndingConnector;

class CircuitOutput : public BaseCircuitItem
{
    Q_OBJECT
public:
    explicit CircuitOutput(const CircuitOutputMimeData& mimeData,
                           QWidget *parent = nullptr);

    inline ItemType GetItemType() const noexcept final
    { return ItemType::Output; }

    virtual void DrawToPixmap() override;

    // Warning: Unused
    void SetValue(bool value) final;

    CircuitOutputMimeData GetMimeData(QPoint eventPos = {}) const;

    void RemoveConnectionId(quint64 connId) override;

signals:
    bool closeDialogs() override;

public slots:
    void SetOrderId(int orderId) final;
    void SetColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_pixmap;
    EndingConnector* m_endingConnector;

    int m_orderId = -1;
    bool m_outputValue = 0;

    QColor m_color = Qt::darkGray;
};

#endif // CIRCUITOUTPUT_HPP
