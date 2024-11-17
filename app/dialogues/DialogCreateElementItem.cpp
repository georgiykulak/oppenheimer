#include "DialogCreateElementItem.hpp"

#include "circuits/items/CircuitElement.hpp"

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

DialogCreateElementItem::DialogCreateElementItem(QPoint pos,
                                                 int orderId,
                                                 QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("New Element");
    setAcceptDrops(false);
    m_minimumHeight = 150;
    setMinimumSize(290, m_minimumHeight);
    move(pos);
    show();
    setAttribute(Qt::WA_DeleteOnClose);

    InitLayout();
    InitElementItem(orderId);
}

void DialogCreateElementItem::SetInputsNumber(int size)
{
    m_newElement->SetInputsNumber(size);
    ResizeWindow();
}

void DialogCreateElementItem::SetOutputsNumber(int size)
{
    m_newElement->SetOutputsNumber(size);
    ResizeWindow();
}

void DialogCreateElementItem::SetOrderIdHint(quint64 itemType, int orderId)
{
    if (itemType != ItemType::Element)
    {
        return;
    }

    m_spinBox->setValue(orderId);
}

void DialogCreateElementItem::dragMoveEvent(QDragMoveEvent *event)
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

void DialogCreateElementItem::dragEnterEvent(QDragEnterEvent *event)
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

void DialogCreateElementItem::mousePressEvent(QMouseEvent *event)
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

void DialogCreateElementItem::InitLayout()
{
    m_itemFrame = new QFrame(this);
    m_itemFrame->setMinimumSize(130, 80);
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

    QLabel* inputsLabel = new QLabel("Inputs", this);
    inputsLabel->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* outputsLabel = new QLabel("Outputs", this);
    outputsLabel->setAttribute(Qt::WA_DeleteOnClose);

    m_inputsCount = new QSpinBox(this);
    m_inputsCount->setRange(1, 16);
    m_inputsCount->setAttribute(Qt::WA_DeleteOnClose);

    m_outputsCount = new QSpinBox(this);
    m_outputsCount->setRange(1, 16);
    m_outputsCount->setAttribute(Qt::WA_DeleteOnClose);

    QSpacerItem* spacer = new QSpacerItem(130, 90,
                                          QSizePolicy::Minimum,
                                          QSizePolicy::Expanding);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(m_itemFrame);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(enterIdLabel);
    vLayout->addWidget(m_spinBox);

    QVBoxLayout* vLeftNestedLayout = new QVBoxLayout;
    vLeftNestedLayout->addWidget(inputsLabel);
    vLeftNestedLayout->addWidget(m_inputsCount);
    QVBoxLayout* vRightNestedLayout = new QVBoxLayout;
    vRightNestedLayout->addWidget(outputsLabel);
    vRightNestedLayout->addWidget(m_outputsCount);
    QHBoxLayout* hNestedLayout = new QHBoxLayout;
    hNestedLayout->addItem(vLeftNestedLayout);
    hNestedLayout->addItem(vRightNestedLayout);

    vLayout->addItem(hNestedLayout);
    vLayout->addItem(spacer);

    hLayout->addItem(vLayout);
    setLayout(hLayout);
}

void DialogCreateElementItem::InitElementItem(int orderId)
{
    QPoint offset(20, 20);
    CircuitElementMimeData mimeData;
    mimeData.orderId = orderId;
    mimeData.endingPoints.push_back({QPoint(5, 55) + offset, 0});
    mimeData.startingPoints.push_back({QPoint(105, 55) + offset,
                                            StartingPoint::IdsSet()});

    m_newElement = new CircuitElement(mimeData, this, false);
    m_newElement->move(offset);

    connect(m_spinBox, qOverload<int>(&QSpinBox::valueChanged),
            m_newElement, &BaseCircuitItem::SetOrderId);

    connect(m_inputsCount, qOverload<int>(&QSpinBox::valueChanged),
            this, &DialogCreateElementItem::SetInputsNumber);

    connect(m_outputsCount, qOverload<int>(&QSpinBox::valueChanged),
            this, &DialogCreateElementItem::SetOutputsNumber);
}

void DialogCreateElementItem::ResizeWindow()
{
    // 40 == (CircuitElement()->height() - m_minimumHeight)
    const auto itemHeight = m_newElement->height() + 40;
    resize(width(), itemHeight);
}
