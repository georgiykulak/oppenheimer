#include "DialogDuplicateElementItem.hpp"
#include "circuits/items/CircuitElement.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QMimeData>
#include <QDrag>
#include <QDragMoveEvent>
#include <QDragEnterEvent>

DialogDuplicateElementItem::DialogDuplicateElementItem(CircuitElement* original,
                                                       int orderId,
                                                       QWidget *parent)
    : QDialog{parent}
    , m_offset{20, 20}
{
    setWindowTitle("Duplicate");
    setAcceptDrops(false);
    setMinimumSize(150, original->GetSize().height() + m_offset.y() * 2);
    show();
    setAttribute(Qt::WA_DeleteOnClose);

    InitLayout();
    InitElementItem(original, orderId);
}

void DialogDuplicateElementItem::SetElementOrderIdHint(int orderId)
{
    if (m_newElement)
    {
        m_newElement->SetOrderId(orderId);
    }
}

void DialogDuplicateElementItem::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(elementMime))
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

void DialogDuplicateElementItem::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(elementMime))
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

void DialogDuplicateElementItem::mousePressEvent(QMouseEvent *event)
{
    QWidget* child = childAt(event->pos());
    if (!child)
        return;

    CircuitElement* circuitElement = qobject_cast<CircuitElement*>(child);
    if (!circuitElement)
        return;

    if (event->button() == Qt::LeftButton)
    {
        QPixmap pixmap = circuitElement->GetPixmap();
        quint64 id = circuitElement->GetId();
        int orderId = circuitElement->GetOrderId();
        int numberParam = circuitElement->GetNumberParameter();
        bool value = circuitElement->GetValue();
        QSize itemSize = circuitElement->GetSize();
        QColor color = circuitElement->GetColor();
        bool isNotationBinary = circuitElement->IsNotationBinary();
        EndingPointVector endPoints = circuitElement->GetEndPoints();
        StartingPointVector startPoints = circuitElement->GetStartPoints();

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream
            << pixmap
            << QPoint(event->pos() - circuitElement->pos())
            << id
            << orderId
            << itemSize
            << value
            << numberParam
            << color
            << isNotationBinary;

        dataStream << (unsigned int)(endPoints.size());
        for (const auto& endPoint : endPoints)
        {
            dataStream << endPoint.connPos << QPoint(event->pos() - endPoint.connPos)
            << endPoint.connId;
        }

        dataStream << (unsigned int)(startPoints.size());
        for (const auto& startPoint : startPoints)
        {
            dataStream << startPoint.connPos << QPoint(event->pos() - startPoint.connPos);

            dataStream << (unsigned int)(startPoint.connIds.size());
            for (const auto& connId : startPoint.connIds)
            {
                dataStream << connId;
            }
        }

        QMimeData *mimeData = new QMimeData;
        mimeData->setData(elementMime, itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->pos() - circuitElement->pos());

        if (drag->exec(Qt::CopyAction) == Qt::CopyAction)
        {
            circuitElement->show();
            circuitElement->SetPixmap(pixmap);
        }
    }

    update();
}

void DialogDuplicateElementItem::InitLayout()
{
    m_itemFrame = new QFrame(this);
    m_itemFrame->setMinimumSize(130, 80);
    m_itemFrame->setFrameShape(QFrame::Box);
    m_itemFrame->setFrameShadow(QFrame::Raised);
    m_itemFrame->setLineWidth(2);
    m_itemFrame->setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(m_itemFrame);
    setLayout(hLayout);
}

void DialogDuplicateElementItem::InitElementItem(CircuitElement* original,
                                                 int orderId)
{
    m_newElement = new CircuitElement(original->GetEndPoints(),
                                      original->GetStartPoints(),
                                      this,
                                      false);

    m_newElement->SetId(original->GetId());
    m_newElement->SetOrderId(orderId);
    m_newElement->SetNumberParameter(original->GetNumberParameter());
    m_newElement->SetValue(original->GetValue());
    m_newElement->SetSize(original->GetSize());
    m_newElement->SetColor(original->GetColor());
    m_newElement->SetNotation(original->IsNotationBinary());
    m_newElement->move(m_offset);
    m_newElement->update();
    m_newElement->show();
    m_newElement->setAttribute(Qt::WA_DeleteOnClose);
}
