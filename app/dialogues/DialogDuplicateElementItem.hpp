#ifndef DIALOGDUPLICATEELEMENTITEM_HPP
#define DIALOGDUPLICATEELEMENTITEM_HPP

#include <QDialog>

class QFrame;
class CircuitElement;

class DialogDuplicateElementItem : public QDialog
{
    Q_OBJECT
public:
    explicit DialogDuplicateElementItem(CircuitElement* original,
                                        int orderId,
                                        QWidget *parent = nullptr);

public slots:
    void SetOrderIdHint(quint64 itemType, int orderId);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;

private:
    QFrame* m_itemFrame;
    CircuitElement* m_newElement;
    const QPoint m_offset;

    void InitLayout();
    void InitElementItem(CircuitElement* original, int orderId);
};

#endif // DIALOGDUPLICATEELEMENTITEM_HPP
