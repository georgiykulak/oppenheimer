#ifndef CIRCUITELEMENT_HPP
#define CIRCUITELEMENT_HPP

#include "BaseCircuitItem.hpp"
#include "mime/CircuitElementMimeData.hpp"

class EndingConnector;
class StartingConnector;
class LogicVectorEdit;
class QPushButton;

class CircuitElement : public BaseCircuitItem
{
    Q_OBJECT
public:
    explicit CircuitElement(const CircuitElementMimeData& mimeData,
                            QWidget *parent = nullptr,
                            bool numParamEnabled = true);

    static void ConstructCircuitElementFromJson(const RequiredItemMeta& reqMeta,
                                                const json& itemMeta,
                                                QWidget* canvas);

    virtual ItemType GetItemType() const noexcept override { return Element; }

    virtual void DrawToPixmap() override;

    void SetNumberParameter(int numParam);
    // Warning: Not used
    virtual void SetValue(bool value) override;
    virtual json GetJsonMeta() const override;

    void SetInputsNumber(int size);
    void SetInputsNumberAndRebook(int size);
    void SetOutputsNumber(int size);
    void SetOutputsNumberAndRebook(int size);
    EndingPointVector GetEndPoints() const;
    StartingPointVector GetStartPoints() const;

    CircuitElementMimeData GetMimeData(QPoint eventPos = {}) const;

    bool IsNumberParameterValid() const;

signals:
    void setNumberParameterToElementItem(quint64 id, int numParam);
    void inputsNumber(int value);
    void outputsNumber(int value);
    void tryToRebookArea(int inputsNumber,
                         int outputsNumber,
                         EndingPointVector endingPoints,
                         StartingPointVector startingPoints,
                         QRect area);

private:
    static constexpr int m_offsetBetweenConnection = 30;

    LogicVectorEdit* m_textField;
    QPushButton* m_notationSwitchButton;

    int m_minimumHeight;
    int m_minimumYShift;

    int m_numberParam = 0;
    bool m_numberParameterIsValid = true;
    bool m_outputValue = 0;
};

#endif // CIRCUITELEMENT_HPP
