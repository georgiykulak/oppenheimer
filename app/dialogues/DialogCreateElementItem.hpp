#ifndef DIALOGCREATEELEMENTITEM_HPP
#define DIALOGCREATEELEMENTITEM_HPP

#include <QDialog>

class QFrame;
class QSpinBox;
class CircuitElement;

class DialogCreateElementItem : public QDialog
{
    Q_OBJECT
public:
    explicit DialogCreateElementItem(QPoint pos,
                                     int orderId,
                                     QWidget *parent = nullptr);
    void SetInputsNumber(int size);
    void SetOutputsNumber(int size);

public slots:
    void SetElementOrderIdHint(int orderId);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

private:
    QFrame* m_itemFrame;
    QSpinBox* m_spinBox;
    QSpinBox* m_inputsCount;
    QSpinBox* m_outputsCount;
    CircuitElement* m_newElement;
    int m_minimumHeight;

    void InitLayout();
    void InitElementItem(int orderId);
    void ResizeWindow(int connectionsOnSideNumber);
};

#endif // DIALOGCREATEELEMENTITEM_HPP
