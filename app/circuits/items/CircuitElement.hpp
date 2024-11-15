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
    ~CircuitElement();

    inline ItemType GetItemType() const noexcept final
    { return ItemType::Element; }

    virtual void DrawToPixmap() override;

    void SetNumberParameter(int numParam);
    // Warning: Not used
    virtual void SetValue(bool value) override;

    void SetInputsNumber(int size);
    void SetInputsNumberAndRebook(int size);
    void SetOutputsNumber(int size);
    void SetOutputsNumberAndRebook(int size);
    EndingPointVector GetEndPoints() const;
    StartingPointVector GetStartPoints() const;

    CircuitElementMimeData GetMimeData(QPoint eventPos = {}) const;

    void RemoveConnectionId(quint64 connId) override;
    int GetOffsetBetweenConnectionPoints() const;
    bool IsNumberParameterValid() const;

signals:
    void setNumberParameterToElementItem(quint64 id, int numParam);
    bool closeDialogs() override;
    void inputsNumber(int value);
    void outputsNumber(int value);
    void tryToRebookArea(int inputsNumber,
                         int outputsNumber,
                         EndingPointVector endingPointVector,
                         StartingPointVector startingPointVector,
                         QRect area);

public slots:
    virtual void SetOrderId(int orderId) override;
    void SetColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_pixmap;
    std::vector<EndingConnector*> m_endingConnectors;
    std::vector<StartingConnector*> m_startingConnectors;
    LogicVectorEdit* m_textField;
    QPushButton* m_notationSwitchButton;

    int m_minimumHeight;
    int m_minimumYShift;
    int m_offsetBetweenConnection = 0;

    int m_numberParam = 0;
    bool m_numberParameterIsValid = true;
    int m_orderId = -1;
    bool m_outputValue = 0;

    QColor m_color = Qt::lightGray;
};

#endif // CIRCUITELEMENT_HPP
