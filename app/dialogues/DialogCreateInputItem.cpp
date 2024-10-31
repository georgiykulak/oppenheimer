#include "DialogCreateInputItem.hpp"

#include "circuits/items/CircuitInput.hpp"

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

DialogCreateInputItem::DialogCreateInputItem(QPoint pos,
                                             int orderId,
                                             QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("New Input");
    setAcceptDrops(false);
    setMinimumSize(230, 70);
    move(pos);
    show();
    setAttribute(Qt::WA_DeleteOnClose);

    InitLayout();
    InitInputItem(orderId);
}

void DialogCreateInputItem::SetOrderId(int orderId)
{
    m_newInput->SetOrderId(orderId);
    m_newInput->update();
}

void DialogCreateInputItem::SetInputOrderIdHint(int orderId)
{
    m_spinBox->setValue(orderId);
}

void DialogCreateInputItem::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(inputMime))
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

void DialogCreateInputItem::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(inputMime))
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

void DialogCreateInputItem::mousePressEvent(QMouseEvent *event)
{
    QWidget* child = childAt(event->pos());
    if (!child)
        return;

    CircuitInput* circuitInput = qobject_cast<CircuitInput*>(child);
    if (!circuitInput)
        return;

    if (event->button() == Qt::LeftButton)
    {
        QPixmap pixmap = circuitInput->GetPixmap();
        quint64 id = circuitInput->GetId();
        int orderId = circuitInput->GetOrderId();
        bool value = circuitInput->GetValue();
        QSize itemSize = circuitInput->GetSize();
        QColor color = circuitInput->GetColor();
        const StartingPoint& startPoint = circuitInput->GetStartPoint();

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream
            << pixmap
            << QPoint(event->pos() - circuitInput->pos())
            << id
            << orderId
            << itemSize
            << value
            << startPoint.connPos
            << QPoint(event->pos() - startPoint.connPos)
            << (unsigned int)(startPoint.connIds.size())
            << color;

        for (const quint64 connId : startPoint.connIds)
        {
            dataStream << connId;
        }

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(inputMime, itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->pos() - circuitInput->pos());

        if (drag->exec(Qt::CopyAction) == Qt::CopyAction)
        {
            circuitInput->show();
            circuitInput->SetPixmap(pixmap);
        }
    }
}

void DialogCreateInputItem::InitLayout()
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
            this, &DialogCreateInputItem::SetOrderId);

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

void DialogCreateInputItem::InitInputItem(int orderId)
{
    QPoint offset(20, 20);
    StartingPoint startPoint;
    startPoint.connPos = QPoint(70, 15) + offset;

    m_newInput = new CircuitInput(startPoint, this);

    m_newInput->SetId(0);
    m_newInput->SetOrderId(orderId);
    m_newInput->SetValue(0);
    m_newInput->move(offset);
    m_newInput->update();
    m_newInput->show();
    m_newInput->setAttribute(Qt::WA_DeleteOnClose);
}
