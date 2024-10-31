#ifndef ELEMENTSIZECHANGER_HPP
#define ELEMENTSIZECHANGER_HPP

#include <QDialog>

class QSpinBox;
class CircuitElement;

class ElementSizeChanger : public QDialog
{
    Q_OBJECT
public:
    explicit ElementSizeChanger(CircuitElement* element, QWidget *parent = nullptr);

private:
    QSpinBox* m_inputsCount;
    QSpinBox* m_outputsCount;
};

#endif // ELEMENTSIZECHANGER_HPP
