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

    inline ItemType GetItemType() const noexcept override { return Output; }

    virtual void DrawToPixmap() override;

    // Warning: Unused
    void SetValue(bool value) final;

    CircuitOutputMimeData GetMimeData(QPoint eventPos = {}) const;

    void RemoveConnectionId(quint64 connId) override;

private:
    bool m_outputValue = 0;
};

#endif // CIRCUITOUTPUT_HPP
