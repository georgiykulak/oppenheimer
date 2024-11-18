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

    static void ConstructCircuitInputFromJson(const RequiredItemMeta& reqMeta,
                                              const json& itemMeta,
                                              QWidget* canvas);

    virtual ItemType GetItemType() const noexcept override { return Input; }

    virtual void DrawToPixmap() override;

    // Warning: Unused
    void SetValue(bool value) final;

    CircuitInputMimeData GetMimeData(QPoint eventPos = {}) const;

    virtual void AddActionsToMenu(QMenu* menu) override;

private:
    bool m_inputValue = 0;
};

#endif // CIRCUITINPUT_HPP
