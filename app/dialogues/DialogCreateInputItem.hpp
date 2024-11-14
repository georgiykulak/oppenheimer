#ifndef DIALOGCREATEINPUTITEM_HPP
#define DIALOGCREATEINPUTITEM_HPP

#include <QDialog>

class QFrame;
class QSpinBox;
class CircuitInput;

class DialogCreateInputItem : public QDialog
{
    Q_OBJECT
public:
    explicit DialogCreateInputItem(QPoint pos,
                                   int orderId,
                                   QWidget *parent = nullptr);

public slots:
    void SetInputOrderIdHint(int orderId);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

private:
    QFrame* m_itemFrame;
    QSpinBox* m_spinBox;
    CircuitInput* m_newInput;

    void InitLayout();
    void InitInputItem(int orderId);
};

#endif // DIALOGCREATEINPUTITEM_HPP
