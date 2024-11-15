#ifndef CIRCUITINPUT_HPP
#define CIRCUITINPUT_HPP

#include "BaseCircuitItem.hpp"
#include "mime/CircuitInputMimeData.hpp"

class StartingConnector;

class CircuitInput : public BaseCircuitItem
{
    Q_OBJECT
public:
    explicit CircuitInput(const CircuitInputMimeData& mimeData,
                          QWidget *parent = nullptr);

    inline ItemType GetItemType() const noexcept final
    { return ItemType::Input; }

    virtual void DrawToPixmap() override;

    // Warning: Unused
    void SetValue(bool value) final;

    CircuitInputMimeData GetMimeData(QPoint eventPos = {}) const;

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
    StartingConnector* m_startingConnector;

    int m_orderId = -1;
    bool m_inputValue = 0;

    QColor m_color = Qt::gray;
};

#endif // CIRCUITINPUT_HPP
