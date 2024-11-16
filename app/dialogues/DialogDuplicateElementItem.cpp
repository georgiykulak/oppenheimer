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
    setMinimumSize(150, original->height() + m_offset.y() * 2);
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
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    QWidget* child = childAt(event->pos());
    if (!child)
    {
        return;
    }

    CircuitElement* circuitElement = qobject_cast<CircuitElement*>(child);
    if (!circuitElement)
    {
        return;
    }

    const auto mimeData = circuitElement->GetMimeData(event->pos());
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << mimeData;

    QMimeData* mime = new QMimeData;
    mime->setData(elementMime, itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mime);
    drag->setPixmap(mimeData.pixmap);
    drag->setHotSpot(event->pos() - circuitElement->pos());

    if (drag->exec(Qt::CopyAction) == Qt::CopyAction)
    {
        circuitElement->show();
    }
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
    auto mimeData = original->GetMimeData({});
    mimeData.orderId = orderId;

    m_newElement = new CircuitElement(mimeData, this, false);
    m_newElement->move(m_offset);
}
