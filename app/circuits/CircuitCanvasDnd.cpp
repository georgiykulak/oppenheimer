#include <circuits/CircuitCanvas.hpp>
#include "items/CircuitInput.hpp"
#include "items/CircuitOutput.hpp"
#include "items/CircuitElement.hpp"
#include "items/connectors/EndingConnector.hpp"
#include "items/connectors/StartingConnector.hpp"
#include "items/mime/CircuitInputMimeData.hpp"
#include "items/mime/CircuitOutputMimeData.hpp"
#include "items/mime/CircuitElementMimeData.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>

void CircuitCanvas::ProcessDragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(inputMime)
        || event->mimeData()->hasFormat(outputMime)
        || event->mimeData()->hasFormat(elementMime))
    {
        AcceptDndEvent(event);
    }
    else if (event->mimeData()->hasFormat(endingConnectorMime)
             && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data(endingConnectorMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint endingPosition;
        QPoint offset;
        quint64 itemId;
        dataStream >> pixmap >> endingPosition >> offset
            >> itemId;

        m_currentConnectingLine = QLine(endingPosition + offset, event->pos());
        update();

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else if (event->mimeData()->hasFormat(startingConnectorMime)
             && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data(startingConnectorMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint startingPosition;
        QPoint offset;
        dataStream >> pixmap >> startingPosition >> offset;

        m_currentConnectingLine = QLine(startingPosition + offset, event->pos());
        update();

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
    {
        m_currentConnectingLine = {};
        event->ignore();
    }
}

void CircuitCanvas::ProcessDragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(inputMime)
        || event->mimeData()->hasFormat(outputMime)
        || event->mimeData()->hasFormat(elementMime))
    {
        AcceptDndEvent(event);
    }
    else if (event->mimeData()->hasFormat(endingConnectorMime)
             && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data(endingConnectorMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint endingPosition;
        QPoint offset;
        quint64 itemId;
        dataStream >> pixmap >> endingPosition >> offset
            >> itemId;

        m_currentConnectingLine = QLine(endingPosition + offset, event->pos());
        update();

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else if (event->mimeData()->hasFormat(startingConnectorMime)
             && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data(startingConnectorMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint startingPosition;
        QPoint offset;
        dataStream >> pixmap >> startingPosition >> offset;

        m_currentConnectingLine = QLine(startingPosition + offset, event->pos());
        update();

        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
    {
        m_currentConnectingLine = {};
        event->ignore();
    }
}

void CircuitCanvas::ProcessDropEvent(QDropEvent* event)
{
    //BaseCircuitItem::GetDropEventProcessorByMimeType()

    /*for (const auto& mimeType : BaseCircuitItem::GetRegisteredMimeTypes())
    {
        if (event->mimeData()->hasFormat(mimeType))
        {
            //QByteArray itemData = event->mimeData()->data(mimeType);
            //QDataStream dataStream(&itemData, QIODevice::ReadOnly);
            //CircuitInputMimeData mimeData(event->pos());
            //dataStream >> mimeData;

            auto itemMeta = BaseCircuitItem::ReadMimeFromByteArray(mimeType);

            if (event->source() != this
                && m_idHandler.ContainsOrderId(itemMeta.type, itemMeta.orderId))
            {
                event->ignore();

                QString text = "Input item with such order id '"
                               + QString::number(itemMeta.orderId) + "' already exists";
                QMessageBox::information(this, "Can't place item", text);

                // TODO: apply backup

                return;
            }

            if (m_areaManager.TryBookArea(itemMeta.area, itemMeta.oldNewPoints))
            {
                if (event->source() != this)
                {
                    qDebug() << "Placing new input item";

                    if (!m_idHandler.NewOrderId(itemMeta.type, itemMeta.orderId))
                    {
                        event->ignore();
                        // TODO: apply backup
                        return;
                    }

                    itemMeta.id = m_idHandler.NewUid();
                    emit addNewItem(itemMeta.type, itemMeta.id,
                                    itemMeta.endingConnectors.size());

                    emit setOrderIdHint(itemMeta.type,
                                        m_idHandler.GetLastOrderId(itemMeta.type));
                }

                //auto circuitInput = new CircuitInput(mimeData, this);
                //circuitInput->move(mimeData.itemPosition);
                BaseCircuitItem::ConstructItemFromByteArray(mimeType, )

                // To redraw connections
                update();

                AcceptDndEvent(event);
            }
            else
            {
                event->ignore();
            }

            return;
        }
    }*/

    if (event->mimeData()->hasFormat(inputMime))
    {
        QByteArray itemData = event->mimeData()->data(inputMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        CircuitInputMimeData mimeData(event->pos());
        dataStream >> mimeData;

        if (event->source() != this
            && m_idHandler.ContainsOrderId(ItemType::Input, mimeData.orderId))
        {
            event->ignore();

            QString text = "Input item with such order id '"
                           + QString::number(mimeData.orderId) + "' already exists";
            QMessageBox::information(this, "Can't place item", text);

            // TODO: apply backup

            return;
        }

        if (m_areaManager.TryBookArea(mimeData.area, mimeData.oldNewPoints))
        {
            if (event->source() != this)
            {
                qDebug() << "Placing new input item";

                if (!m_idHandler.NewOrderId(ItemType::Input, mimeData.orderId))
                {
                    event->ignore();
                    // TODO: apply backup
                    return;
                }

                mimeData.id = m_idHandler.NewUid();
                emit addNewItem(ItemType::Input, mimeData.id, 0);

                emit setOrderIdHint(ItemType::Input,
                                    m_idHandler.GetLastOrderId(ItemType::Input));
            }

            auto item = new CircuitInput(mimeData, this);
            item->move(mimeData.itemPosition);

            connect(item, &BaseCircuitItem::removeCircuitItem,
                    this, &CircuitCanvas::RemoveCircuitItem);

            // To redraw connections
            update();

            AcceptDndEvent(event);
        }
        else
        {
            event->ignore();
        }
    }
    else if (event->mimeData()->hasFormat(outputMime))
    {
        QByteArray itemData = event->mimeData()->data(outputMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        CircuitOutputMimeData mimeData(event->pos());
        dataStream >> mimeData;

        if (event->source() != this
            && m_idHandler.ContainsOrderId(ItemType::Output, mimeData.orderId))
        {
            event->ignore();

            QString text = "Output item with such order id '"
                           + QString::number(mimeData.orderId) + "' already exists";
            QMessageBox::information(this, "Can't place item", text);

            // TODO: apply backup

            return;
        }

        if (m_areaManager.TryBookArea(mimeData.area, mimeData.oldNewPoints))
        {
            if (event->source() != this)
            {
                qDebug() << "Placing new output item";

                if (!m_idHandler.NewOrderId(ItemType::Output, mimeData.orderId))
                {
                    event->ignore();
                    // TODO: apply backup
                    return;
                }

                mimeData.id = m_idHandler.NewUid();
                emit addNewItem(ItemType::Output, mimeData.id, 1);

                emit setOrderIdHint(ItemType::Output,
                                    m_idHandler.GetLastOrderId(ItemType::Output));
            }

            auto* item = new CircuitOutput(mimeData, this);
            item->move(mimeData.itemPosition);

            connect(item, &BaseCircuitItem::removeCircuitItem,
                    this, &CircuitCanvas::RemoveCircuitItem);

            // To redraw connections
            update();

            AcceptDndEvent(event);
        }
        else
        {
            event->ignore();
        }
    }
    else if (event->mimeData()->hasFormat(elementMime))
    {
        QByteArray itemData = event->mimeData()->data(elementMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        CircuitElementMimeData mimeData(event->pos());
        dataStream >> mimeData;

        if (event->source() != this
            && m_idHandler.ContainsOrderId(ItemType::Element, mimeData.orderId))
        {
            event->ignore();

            QString text = "Element item with such order id '"
                           + QString::number(mimeData.orderId) + "' already exists";
            QMessageBox::information(this, "Can't place item", text);

            // TODO: apply backup

            return;
        }

        if (m_areaManager.TryBookArea(mimeData.area, mimeData.oldNewPoints))
        {
            if (event->source() != this)
            {
                qDebug() << "Placing new output item";

                if (!m_idHandler.NewOrderId(ItemType::Element, mimeData.orderId))
                {
                    event->ignore();
                    // TODO: apply backup
                    return;
                }

                mimeData.id = m_idHandler.NewUid();
                emit addNewItem(ItemType::Element, mimeData.id,
                                mimeData.endingPoints.size());

                emit setOrderIdHint(ItemType::Element,
                                    m_idHandler.GetLastOrderId(ItemType::Element));
            }

            auto* item = new CircuitElement(mimeData, this);
            item->move(mimeData.itemPosition);

            connect(item, &BaseCircuitItem::removeCircuitItem,
                    this, &CircuitCanvas::RemoveCircuitItem);

            connect(item, &CircuitElement::setNumberParameterToElementItem,
                    this, &CircuitCanvas::setNumberParameterToElementItem);

            connect(item, &CircuitElement::startFunctionalFaultSimulation,
                    this, &CircuitCanvas::startFunctionalFaultSimulation);

            connect(item, &CircuitElement::askOrderIdHint,
                    this, [this](){
                        const auto orderId =
                            m_idHandler.GetLastOrderId(ItemType::Element);
                        emit setOrderIdHint(ItemType::Element, orderId);
                    });

            connect(this, &CircuitCanvas::setOrderIdHint,
                    item, &CircuitElement::setOrderIdHintForDuplicate);

            connect(item, &CircuitElement::tryToRebookArea,
                    this, &CircuitCanvas::TryToRebookArea);

            // To redraw connections
            update();

            AcceptDndEvent(event);
        }
        else
        {
            event->ignore();
        }
    }
    else if (event->mimeData()->hasFormat(endingConnectorMime)
             && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data(endingConnectorMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint sourceItemPosition;
        QPoint offset;
        quint64 sourceItemId;
        dataStream >> pixmap >> sourceItemPosition >> offset >> sourceItemId;

        m_currentConnectingLine = {};
        QWidget* child = childAt(event->pos());
        StartingConnector* startingConnector =
            qobject_cast<StartingConnector*>(child);
        if (!child || !startingConnector)
        {
            update();
            event->ignore();
            return;
        }

        qDebug() << "Create new connection, initiated by STARTING connector";

        const auto startId = startingConnector->GetItemId();
        const auto startPos = startingConnector->GetStartPoint().connPos;
        const auto endPos = sourceItemPosition + offset;
        const auto positions = QLine(startPos, endPos);

        InsertConnection(startId, sourceItemId, positions);

        update();
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else if (event->mimeData()->hasFormat(startingConnectorMime)
             && event->source() == this)
    {
        QByteArray itemData = event->mimeData()->data(startingConnectorMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint sourceItemPosition;
        QPoint offset;
        quint64 sourceItemId;
        dataStream >> pixmap >> sourceItemPosition >> offset >> sourceItemId;

        m_currentConnectingLine = {};
        QWidget* child = childAt(event->pos());
        EndingConnector* endingConnector =
            qobject_cast<EndingConnector*>(child);
        if (!child || !endingConnector)
        {
            update();
            event->ignore();
            return;
        }

        qDebug() << "Create new connection, initiated by ENDING connector";

        if (endingConnector->IsConnected())
        {
            qDebug() << "Can't connect: busy by connection, id ="
                     << endingConnector->GetConnectionId();

            update();
            event->ignore();
            return;
        }

        const auto endId = endingConnector->GetItemId();
        const auto endPos = endingConnector->GetEndPoint().connPos;
        const auto startPos = sourceItemPosition + offset;
        const auto positions = QLine(startPos, endPos);

        InsertConnection(sourceItemId, endId, positions);

        update();
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
    else
    {
        m_currentConnectingLine = {};
        event->ignore();
    }
}

void CircuitCanvas::ProcessMousePressEvent(QMouseEvent *event)
{
    QWidget* child = childAt(event->pos());
    if (!child)
    {
        return;
    }

    /*auto* item = qobject_cast<BaseCircuitItem*>(child);
    if (item)
    {
        if (event->button() == Qt::LeftButton)
        {
            //const auto mimeData = item->GetMimeData(event->pos());
            //QByteArray itemData;
            //QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            //dataStream << mimeData;

            auto itemData = item->WriteMimeByteArray();

            QMimeData* mime = new QMimeData;
            mime->setData(item->GetMimeType(), itemData);

            QDrag* drag = new QDrag(this);
            drag->setMimeData(mime);
            drag->setPixmap(mimeData.pixmap);
            drag->setHotSpot(event->pos() - item->pos());

            m_areaManager.ClearAndBackupArea(mimeData.area);

            if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
            {
                item->close();
            }
            else
            {
                item->show();
            }
        }
        else if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton)
        {
            QMenu* menu = new QMenu(this);

            item->AddActionsToMenu(menu, this);

            menu->move(mapToGlobal(event->pos()));
            menu->show();
            menu->setAttribute(Qt::WA_DeleteOnClose);
        }
        else
        {
            //item->ProcessMousePressEvent(event);
        }

        return;
    }*/

    CircuitInput* circuitInput = qobject_cast<CircuitInput*>(child);
    if (circuitInput)
    {
        if (event->button() == Qt::LeftButton)
        {
            const auto mimeData = circuitInput->GetMimeData(event->pos());
            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << mimeData;

            QMimeData* mime = new QMimeData;
            mime->setData(inputMime, itemData);

            QDrag* drag = new QDrag(this);
            drag->setMimeData(mime);
            drag->setPixmap(mimeData.pixmap);
            drag->setHotSpot(event->pos() - circuitInput->pos());

            m_areaManager.ClearAndBackupArea(mimeData.area);

            if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
            {
                circuitInput->close();
            }
            else
            {
                circuitInput->show();
            }
        }
        else if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton)
        {
            QMenu* menu = new QMenu(this);

            circuitInput->AddActionsToMenu(menu);

            menu->move(mapToGlobal(event->pos()));
            menu->show();
            menu->setAttribute(Qt::WA_DeleteOnClose);
        }

        return;
    }

    CircuitOutput* circuitOutput = qobject_cast<CircuitOutput*>(child);
    if (circuitOutput)
    {
        if (event->button() == Qt::LeftButton)
        {
            const auto mimeData = circuitOutput->GetMimeData(event->pos());
            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << mimeData;

            QMimeData* mime = new QMimeData;
            mime->setData(outputMime, itemData);

            QDrag* drag = new QDrag(this);
            drag->setMimeData(mime);
            drag->setPixmap(mimeData.pixmap);
            drag->setHotSpot(event->pos() - circuitOutput->pos());

            m_areaManager.ClearAndBackupArea(mimeData.area);

            if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
            {
                circuitOutput->close();
            }
            else
            {
                circuitOutput->show();
            }
        }
        else if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton)
        {
            QMenu* menu = new QMenu(this);

            circuitOutput->AddActionsToMenu(menu);

            menu->move(mapToGlobal(event->pos()));
            menu->show();
            menu->setAttribute(Qt::WA_DeleteOnClose);
        }

        return;
    }

    CircuitElement* circuitElement = qobject_cast<CircuitElement*>(child);
    if (circuitElement)
    {
        if (event->button() == Qt::LeftButton)
        {
            emit circuitElement->closeDialogs();

            const auto mimeData = circuitElement->GetMimeData(event->pos());
            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << mimeData;

            QMimeData* mime = new QMimeData;
            mime->setData(elementMime, itemData);

            QDrag* drag = new QDrag(this);
            drag->setMimeData(mime);
            drag->setPixmap(mimeData.pixmap);
            drag->setHotSpot(event->pos() - circuitElement->pos());

            m_areaManager.ClearAndBackupArea(mimeData.area);

            if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
            {
                circuitElement->close();
            }
            else
            {
                circuitElement->show();
            }
        }
        else if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton)
        {
            QMenu* menu = new QMenu(this);

            circuitElement->AddActionsToMenu(menu);

            menu->move(mapToGlobal(event->pos()));
            menu->show();
            menu->setAttribute(Qt::WA_DeleteOnClose);
        }

        return;
    }

    EndingConnector* endingConnector = qobject_cast<EndingConnector*>(child);
    if (endingConnector)
    {
        if (event->button() == Qt::LeftButton && !endingConnector->IsConnected())
        {
            QPixmap pixmap = endingConnector->GetPixmap();
            QPoint endingPosition = endingConnector->GetEndPoint().connPos;
            quint64 itemId = endingConnector->GetItemId();
            QPoint offset = {};

            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << pixmap << endingPosition << offset
                       << itemId;

            QMimeData *mimeData = new QMimeData;
            mimeData->setData("application/x-oph-endingconnector", itemData);

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setPixmap(pixmap);
            drag->setHotSpot(offset + endingConnector->GetConnectorSize());

            //! TODO: improve design for dragging element
            QPixmap tempPixmap = pixmap;
            QPainter painter;
            painter.begin(&tempPixmap);
            painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 50));
            painter.end();
            //!

            endingConnector->SetPixmap(tempPixmap);

            if (drag->exec(Qt::CopyAction) == Qt::CopyAction)
            {
                endingConnector->show();
                endingConnector->SetPixmap(pixmap);
            }

            return;
        }
        else if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton)
        {
            QMenu* menu = new QMenu(this);
            QAction* actionDisconnect = new QAction("Disconnect", this);
            connect(actionDisconnect, &QAction::triggered,
                    this, [this, endingConnector] (bool) {
                        const auto connId = endingConnector->GetConnectionId();

                        RemoveConnectionById(connId);

                        update();
                    });

            menu->addAction(actionDisconnect);

            menu->move(mapToGlobal(event->pos()));
            menu->show();
            menu->setAttribute(Qt::WA_DeleteOnClose);
        }
    }

    StartingConnector* startingConnector = qobject_cast<StartingConnector*>(child);
    if (startingConnector)
    {
        if (event->button() == Qt::LeftButton)
        {
            QPixmap pixmap = startingConnector->GetPixmap();
            QPoint startingPosition = startingConnector->GetStartPoint().connPos;
            quint64 itemId = startingConnector->GetItemId();
            QPoint offset = {};

            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << pixmap << startingPosition << offset
                       << itemId;

            QMimeData *mimeData = new QMimeData;
            mimeData->setData("application/x-oph-startingconnector", itemData);

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setPixmap(pixmap);
            drag->setHotSpot(offset + startingConnector->GetConnectorSize());

            //! TODO: improve design for dragging element
            QPixmap tempPixmap = pixmap;
            QPainter painter;
            painter.begin(&tempPixmap);
            painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 50));
            painter.end();
            //!

            startingConnector->SetPixmap(tempPixmap);

            if (drag->exec(Qt::CopyAction) == Qt::CopyAction)
            {
                startingConnector->show();
                startingConnector->SetPixmap(pixmap);
            }

            return;
        }
        else if (event->button() == Qt::RightButton || event->button() == Qt::MiddleButton)
        {
            QMenu* menu = new QMenu(this);
            const auto connIdsSize = startingConnector->GetStartPoint().connIds.size();
            QAction* actionDisconnect = new QAction(connIdsSize > 1 ? "Disconnect All" : "Disconnect", this);
            connect(actionDisconnect, &QAction::triggered,
                    this, [this, startingConnector] (bool) {
                        const auto connIdSet = startingConnector->GetStartPoint().connIds;
                        for (const auto& connId : connIdSet)
                        {
                            RemoveConnectionById(connId);
                        }

                        update();
                    });

            menu->addAction(actionDisconnect);

            menu->move(mapToGlobal(event->pos()));
            menu->show();
            menu->setAttribute(Qt::WA_DeleteOnClose);
        }
    }
}

void CircuitCanvas::AcceptDndEvent(QDropEvent* baseDndEvent)
{
    if (baseDndEvent->source() == this)
    {
        baseDndEvent->setDropAction(Qt::MoveAction);
        baseDndEvent->accept();
    }
    else
    {
        baseDndEvent->acceptProposedAction();
    }
}

void CircuitCanvas::RemoveConnectionById(quint64 connId)
{
    qDebug() << "CircuitCanvas::RemoveConnectionById: Trying to remove connection, ID ="
             << connId;

    m_areaManager.RemoveConnection(connId);

    QObjectList childList = this->children();

    for (auto* obj : childList)
    {
        auto* item = qobject_cast<BaseCircuitItem*>(obj);
        if (item)
        {
            item->RemoveConnectionId(connId);
        }
    }
}

void CircuitCanvas::RemoveCircuitItem(BaseCircuitItem* item)
{
    if (!item)
    {
        return;
    }

    emit item->closeDialogs();

    const auto uid = item->GetId();
    emit removeItem(uid);
    m_idHandler.RemoveUid(item->GetId());

    /*
    const auto mimeData = item->GetMimeData();
    m_idHandler.RemoveOrderId(item->GetItemType(), mimeData.orderId);

    for (const auto* endingConnector : item->GetEndingConnectors())
    {
        const auto connId = endingConnector->GetEndPoint().connId;
        RemoveConnectionById(connId);
    }

    for (const auto* startingConnector : item->GetStartingConnectors())
    {
        const auto& connIdSet = startingConnector->GetStartPoint().connIds;
        for (const auto& connId : connIdSet)
        {
            RemoveConnectionById(connId);
        }
    }
    */

    ///*
    if (auto* circuitInput = qobject_cast<CircuitInput*>(item); circuitInput)
    {
        const auto mimeData = circuitInput->GetMimeData();
        m_idHandler.RemoveOrderId(ItemType::Input, mimeData.orderId);

        const auto& startPoints = mimeData.startingPoints;
        for (const auto& startPoint : startPoints)
        {
            const auto& connIdSet = startPoint.connIds;
            for (const auto& connId : connIdSet)
            {
                RemoveConnectionById(connId);
            }
        }

        const auto& endPoints = mimeData.endingPoints;
        for (const auto& endPoint : endPoints)
        {
            const auto connId = endPoint.connId;
            RemoveConnectionById(connId);
        }
    }
    else if (auto* circuitOutput = qobject_cast<CircuitOutput*>(item); circuitOutput)
    {
        const auto mimeData = circuitOutput->GetMimeData();
        m_idHandler.RemoveOrderId(ItemType::Output, mimeData.orderId);

        const auto& startPoints = mimeData.startingPoints;
        for (const auto& startPoint : startPoints)
        {
            const auto& connIdSet = startPoint.connIds;
            for (const auto& connId : connIdSet)
            {
                RemoveConnectionById(connId);
            }
        }

        const auto& endPoints = mimeData.endingPoints;
        for (const auto& endPoint : endPoints)
        {
            const auto connId = endPoint.connId;
            RemoveConnectionById(connId);
        }
    }
    else if (auto* circuitElement = qobject_cast<CircuitElement*>(item); circuitElement)
    {
        const auto mimeData = circuitElement->GetMimeData();
        m_idHandler.RemoveOrderId(ItemType::Element, mimeData.orderId);

        const auto& startPoints = mimeData.startingPoints;
        for (const auto& startPoint : startPoints)
        {
            const auto& connIdSet = startPoint.connIds;
            for (const auto& connId : connIdSet)
            {
                RemoveConnectionById(connId);
            }
        }

        const auto& endPoints = mimeData.endingPoints;
        for (const auto& endPoint : endPoints)
        {
            const auto connId = endPoint.connId;
            RemoveConnectionById(connId);
        }
    }
    else
    {
        throw std::runtime_error("ItemType is unknown, type = "
                                 + std::to_string(item->GetItemType()));
    }
    //*/

    QRect area(item->pos(), item->size());
    m_areaManager.ClearArea(area);

    item->close();

    update();
}

void CircuitCanvas::InsertConnection(quint64 startId,
                                     quint64 endId,
                                     QLine positions)
{
    qDebug() << "InsertConnection: Trying to add connection"
                " between (starting) item ID =" << startId
             << "and (ending) item ID =" << endId
             << "--> line from" << positions.p1() << "to" << positions.p2();

    if (!startId || !endId)
    {
        qDebug() << "InsertConnection: zero id detected, aborting";
        return;
    }

    const auto connId =
        m_areaManager.AddConnection(positions, startId, endId);

    if (!connId)
    {
        qDebug() << "InsertConnection: Can't create connection with ID"
                 << connId;
        return;
    }

    const auto startPos = positions.p1();
    const auto endPos = positions.p2();
    QObjectList childList = this->children();

    std::size_t connectorsFound = 0;
    for (auto* obj : childList)
    {
        auto* item = qobject_cast<BaseCircuitItem*>(obj);
        if (!item)
        {
            continue;
        }

        if (item->GetId() == startId)
        {
            const auto connectors = item->GetStartingConnectors();
            for (auto* connector : connectors)
            {
                if (connector && connector->GetStartPoint().connPos == startPos)
                {
                    connector->SetConnectionId(connId);
                    ++connectorsFound;
                    break;
                }
            }
        }

        if (item->GetId() == endId)
        {
            const auto connectors = item->GetEndingConnectors();
            for (auto* connector : connectors)
            {
                if (connector && connector->GetEndPoint().connPos == endPos)
                {
                    connector->SetConnectionId(connId);
                    ++connectorsFound;
                    break;
                }
            }
        }

        if (connectorsFound == 2)
        {
            break;
        }
    }
}

void CircuitCanvas::ClearAll()
{
    QObjectList childList = this->children();
    for (auto* obj : childList)
    {
        auto* item = qobject_cast<BaseCircuitItem*>(obj);
        if (item)
        {
            RemoveCircuitItem(item);
        }
    }

    m_idHandler.Clear();

    m_areaManager.RemoveAllConnections();
    m_areaManager.ClearMatrix();

    emit clearAllItems();

    update();
}
