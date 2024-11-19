#ifndef CIRCUITINPUT_HPP
#define CIRCUITINPUT_HPP

#include "BaseCircuitItem.hpp"
#include "mime/CircuitInputMimeData.hpp"

class StartingConnector;
class ItemRegistry;

class CircuitInput : public BaseCircuitItem
{
    Q_OBJECT
public:
    explicit CircuitInput(const CircuitInputMimeData& mimeData,
                          QWidget *parent = nullptr);

    static void ConstructCircuitInputFromJson(const RequiredItemMeta& reqMeta,
                                              const json& itemMeta,
                                              QWidget* canvas);

    static void ConstructCircuitInputFromStream(const BaseCircuitItemMimeData& baseMimeData,
                                                QDataStream& additionalData,
                                                ItemRegistry* itemRegistry);

    virtual ItemType GetItemType() const noexcept override { return Input; }
    virtual QString GetMimeType() const override { return inputMime; }

    virtual void DrawToPixmap() override;

    // Warning: Unused
    void SetValue(bool value) final;

    CircuitInputMimeData GetMimeData(QPoint eventPos = {}) const;

    virtual void AddActionsToMenu(QMenu* menu) override;

private:
    bool m_inputValue = 0;
};

#endif // CIRCUITINPUT_HPP
