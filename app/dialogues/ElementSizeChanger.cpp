#include "ElementSizeChanger.hpp"

#include "circuits/items/CircuitElement.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QSpinBox>

ElementSizeChanger::ElementSizeChanger(CircuitElement* element, QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("Change Size");
    show();
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* inputsLabel = new QLabel("Inputs", this);
    inputsLabel->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* outputsLabel = new QLabel("Outputs", this);
    outputsLabel->setAttribute(Qt::WA_DeleteOnClose);

    m_inputsCount = new QSpinBox(this);
    m_inputsCount->setRange(1, 16);
    m_inputsCount->setValue(element->GetEndPoints().size());
    m_inputsCount->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_inputsCount, qOverload<int>(&QSpinBox::valueChanged),
            element, &CircuitElement::SetInputsNumberAndRebook);
    connect(element, &CircuitElement::inputsNumber,
            m_inputsCount, &QSpinBox::setValue);

    m_outputsCount = new QSpinBox(this);
    m_outputsCount->setRange(1, 16);
    m_outputsCount->setValue(element->GetStartPoints().size());
    m_outputsCount->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_outputsCount, qOverload<int>(&QSpinBox::valueChanged),
            element, &CircuitElement::SetOutputsNumberAndRebook);
    connect(element, &CircuitElement::outputsNumber,
            m_outputsCount, &QSpinBox::setValue);

    QVBoxLayout* vLeftNestedLayout = new QVBoxLayout;
    vLeftNestedLayout->addWidget(inputsLabel);
    vLeftNestedLayout->addWidget(m_inputsCount);
    QVBoxLayout* vRightNestedLayout = new QVBoxLayout;
    vRightNestedLayout->addWidget(outputsLabel);
    vRightNestedLayout->addWidget(m_outputsCount);
    QHBoxLayout* hNestedLayout = new QHBoxLayout;
    hNestedLayout->addItem(vLeftNestedLayout);
    hNestedLayout->addItem(vRightNestedLayout);

    setLayout(hNestedLayout);
}
