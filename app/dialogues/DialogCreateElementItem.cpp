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

void DialogCreateElementItem::SetOrderId(int orderId)
{
    m_newElement->SetOrderId(orderId);
    m_newElement->update();
}

void DialogCreateElementItem::SetInputsNumber(int size)
{
    m_newElement->SetInputsNumber(size);
    m_newElement->update();
    ResizeWindow(size);
}

void DialogCreateElementItem::SetOutputsNumber(int size)
{
    m_newElement->SetOutputsNumber(size);
    m_newElement->update();
    ResizeWindow(size);
}

void DialogCreateElementItem::SetElementOrderIdHint(int orderId)
{
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

    connect(m_spinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &DialogCreateElementItem::SetOrderId);

    QLabel* inputsLabel = new QLabel("Inputs", this);
    inputsLabel->setAttribute(Qt::WA_DeleteOnClose);

    QLabel* outputsLabel = new QLabel("Outputs", this);
    outputsLabel->setAttribute(Qt::WA_DeleteOnClose);

    m_inputsCount = new QSpinBox(this);
    m_inputsCount->setRange(1, 16);
    m_inputsCount->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_inputsCount, qOverload<int>(&QSpinBox::valueChanged),
            this, &DialogCreateElementItem::SetInputsNumber);

    m_outputsCount = new QSpinBox(this);
    m_outputsCount->setRange(1, 16);
    m_outputsCount->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_outputsCount, qOverload<int>(&QSpinBox::valueChanged),
            this, &DialogCreateElementItem::SetOutputsNumber);

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
    EndingPointVector endPoints;
    endPoints.push_back({QPoint(5, 55) + offset, 0});
    StartingPointVector startPoints;
    startPoints.push_back({QPoint(105, 55) + offset, StartingPoint::IdsSet()});

    m_newElement = new CircuitElement(endPoints,
                                      startPoints,
                                      this);

    m_newElement->SetId(0);
    m_newElement->SetOrderId(orderId);
    m_newElement->SetNumberParameter(0);
    m_newElement->SetValue(orderId % 2);
    m_newElement->move(offset);
    m_newElement->update();
    m_newElement->show();
    m_newElement->setAttribute(Qt::WA_DeleteOnClose);
}

void DialogCreateElementItem::ResizeWindow(int connectionsOnSideNumber)
{
    const auto hOffset = m_newElement->GetOffsetBetweenConnectionPoints();
    auto newHeight = m_minimumHeight + (connectionsOnSideNumber - 1) * hOffset;
    // 40 == (CircuitElement()->height() - m_minimumHeight)
    const auto itemHeight = m_newElement->height() + 40;

    if (itemHeight > newHeight)
    {
        newHeight = itemHeight;
    }

    if (newHeight > height())
    {
        resize(QSize(width(), newHeight));
    }
}
