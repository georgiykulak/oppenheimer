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

    inline ItemType GetItemType() const noexcept override { return Input; }

    virtual void DrawToPixmap() override;

    // Warning: Unused
    void SetValue(bool value) final;

    CircuitInputMimeData GetMimeData(QPoint eventPos = {}) const;

    void RemoveConnectionId(quint64 connId) override;

private:
    bool m_inputValue = 0;
};

#endif // CIRCUITINPUT_HPP
