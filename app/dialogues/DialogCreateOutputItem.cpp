#include "DialogCreateOutputItem.hpp"

#include "circuits/items/CircuitOutput.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QSpinBox>
#include <QSpacerItem>
#include <QPainter>
#include <QMimeData>
#include <QDrag>
#include <QDragMoveEvent>
#include <QDragEnterEvent>

DialogCreateOutputItem::DialogCreateOutputItem(QPoint pos,
                                               int orderId,
                                               QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("New Output");
    setAcceptDrops(false);
    setMinimumSize(230, 70);
    move(pos);
    show();
    setAttribute(Qt::WA_DeleteOnClose);

    InitLayout();
    InitOutputItem(orderId);
}

void DialogCreateOutputItem::SetOrderId(int orderId)
{
    m_newOutput->SetOrderId(orderId);
    m_newOutput->update();
}

void DialogCreateOutputItem::SetOutputOrderIdHint(int orderId)
{
    m_spinBox->setValue(orderId);
}

void DialogCreateOutputItem::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(outputMime))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::CopyAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void DialogCreateOutputItem::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(outputMime))
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::CopyAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void DialogCreateOutputItem::mousePressEvent(QMouseEvent *event)
{
    QWidget* child = childAt(event->pos());
    if (!child)
        return;

    CircuitOutput* circuitOutput = qobject_cast<CircuitOutput*>(child);
    if (!circuitOutput)
        return;

    if (event->button() == Qt::LeftButton)
    {
        QPixmap pixmap = circuitOutput->GetPixmap();
        quint64 id = circuitOutput->GetId();
        int orderId = circuitOutput->GetOrderId();
        bool value = circuitOutput->GetValue();
        QSize itemSize = circuitOutput->GetSize();
        QColor color = circuitOutput->GetColor();
        const EndingPoint& endPoint = circuitOutput->GetEndPoint();

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream
            << pixmap
            << QPoint(event->pos() - circuitOutput->pos())
            << id
            << orderId
            << itemSize
            << value
            << endPoint.connPos
            << QPoint(event->pos() - endPoint.connPos)
            << color
            << endPoint.connId;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(outputMime, itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->pos() - circuitOutput->pos());

        if (drag->exec(Qt::CopyAction) == Qt::CopyAction)
        {
            circuitOutput->show();
            circuitOutput->SetPixmap(pixmap);
        }
    }
}

void DialogCreateOutputItem::InitLayout()
{
    m_itemFrame = new QFrame(this);
    m_itemFrame->setMinimumSize(100, 40);
    m_itemFrame->setFrameShape(QFrame::Box);
    m_itemFrame->setFrameShadow(QFrame::Raised);
    m_itemFrame->setLineWidth(2);
    m_itemFrame->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* enterIdLabel = new QLabel("Enter ID:", this);
    enterIdLabel->setMinimumWidth(60);
    enterIdLabel->setMaximumWidth(60);
    enterIdLabel->setAttribute(Qt::WA_DeleteOnClose);

    m_spinBox = new QSpinBox(this);
    m_spinBox->setMinimumWidth(60);
    m_spinBox->setMaximumWidth(60);
    m_spinBox->setMaximum(9999);
    m_spinBox->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_spinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &DialogCreateOutputItem::SetOrderId);

    QSpacerItem* spacer = new QSpacerItem(120, 90,
                                          QSizePolicy::Minimum,
                                          QSizePolicy::Expanding);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(m_itemFrame);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(enterIdLabel);
    vLayout->addWidget(m_spinBox);
    vLayout->addItem(spacer);

    hLayout->addItem(vLayout);
    setLayout(hLayout);
}

void DialogCreateOutputItem::InitOutputItem(int orderId)
{
    QPoint offset(20, 20);
    EndingPoint endPoint;
    endPoint.connPos = QPoint(5, 15) + offset;

    m_newOutput = new CircuitOutput(endPoint, this);

    m_newOutput->SetId(0);
    m_newOutput->SetOrderId(orderId);
    m_newOutput->SetValue(orderId % 2);
    m_newOutput->move(offset);
    m_newOutput->update();
    m_newOutput->show();
    m_newOutput->setAttribute(Qt::WA_DeleteOnClose);
}
