#ifndef DIALOGCREATEOUTPUTITEM_HPP
#define DIALOGCREATEOUTPUTITEM_HPP

#include <QDialog>

class QFrame;
class QSpinBox;
class CircuitOutput;

class DialogCreateOutputItem : public QDialog
{
    Q_OBJECT
public:
    explicit DialogCreateOutputItem(QPoint pos,
                                   int orderId,
                                   QWidget *parent = nullptr);
    void SetOrderId(int orderId);

public slots:
    void SetOutputOrderIdHint(int orderId);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

private:
    QFrame* m_itemFrame;
    QSpinBox* m_spinBox;
    CircuitOutput* m_newOutput;

    void InitLayout();
    void InitOutputItem(int orderId);
};

#endif // DIALOGCREATEOUTPUTITEM_HPP
