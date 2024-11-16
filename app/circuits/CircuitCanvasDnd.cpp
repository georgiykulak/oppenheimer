#include <circuits/CircuitCanvas.hpp>
#include "items/CircuitInput.hpp"
#include "items/CircuitOutput.hpp"
#include "items/CircuitElement.hpp"
#include "items/connectors/EndingConnector.hpp"
#include "items/connectors/StartingConnector.hpp"
#include "items/mime/CircuitInputMimeData.hpp"
#include "items/mime/CircuitOutputMimeData.hpp"
#include "items/mime/CircuitElementMimeData.hpp"
#include "dialogues/ElementSizeChanger.hpp"
#include "dialogues/DialogDuplicateElementItem.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QColorDialog>

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

void CircuitCanvas::ProcessDropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(inputMime))
    {
        QByteArray itemData = event->mimeData()->data(inputMime);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        CircuitInputMimeData mimeData(event->pos());
        dataStream >> mimeData;

        if (event->source() != this
            && m_idHandler.ContainsInputOrderId(mimeData.orderId))
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

                if (!m_idHandler.NewInputOrderId(mimeData.orderId))
                {
                    event->ignore();
                    // TODO: apply backup
                    return;
                }

                mimeData.id = m_idHandler.NewUid();
                emit addNewInputItem(mimeData.id);

                emit setInputOrderIdHint(m_idHandler.GetLastInputOrderId());
            }

            auto circuitInput = new CircuitInput(mimeData, this);
            circuitInput->move(mimeData.itemPosition);

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
            && m_idHandler.ContainsOutputOrderId(mimeData.orderId))
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

                if (!m_idHandler.NewOutputOrderId(mimeData.orderId))
                {
                    event->ignore();
                    // TODO: apply backup
                    return;
                }

                mimeData.id = m_idHandler.NewUid();
                emit addNewOutputItem(mimeData.id);

                emit setOutputOrderIdHint(m_idHandler.GetLastOutputOrderId());
            }

            CircuitOutput *cOutput = new CircuitOutput(mimeData, this);
            cOutput->move(mimeData.itemPosition);

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
            && m_idHandler.ContainsElementOrderId(mimeData.orderId))
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

                if (!m_idHandler.NewElementOrderId(mimeData.orderId))
                {
                    event->ignore();
                    // TODO: apply backup
                    return;
                }

                mimeData.id = m_idHandler.NewUid();
                emit addNewElementItem(mimeData.id, mimeData.endingPointVector.size());

                emit setElementOrderIdHint(m_idHandler.GetLastElementOrderId());
            }

            CircuitElement *cElement = new CircuitElement(mimeData, this);
            cElement->move(mimeData.itemPosition);

            connect(cElement, &CircuitElement::setNumberParameterToElementItem,
                    this, &CircuitCanvas::setNumberParameterToElementItem);

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
            QAction* actionChangeColor = new QAction("Change Color", this);
            connect(actionChangeColor, &QAction::triggered,
                    this, [this, circuitInput] (bool) {
                        emit circuitInput->closeDialogs();

                        auto* colorDialog = new QColorDialog(this);
                        colorDialog->setOptions(QColorDialog::DontUseNativeDialog);
                        colorDialog->show();
                        colorDialog->setAttribute(Qt::WA_DeleteOnClose);

                        connect(colorDialog, &QColorDialog::colorSelected,
                                circuitInput, &CircuitInput::SetColor);
                        connect(circuitInput, &CircuitInput::closeDialogs,
                                colorDialog, &QColorDialog::close);
                    });

            QAction* actionDelete = new QAction("Delete", this);
            connect(actionDelete, &QAction::triggered,
                    this, [this, circuitInput] (bool) {
                        qDebug() << "Action Delete for circuit input invoked, ID ="
                                 << circuitInput->GetId();

                        RemoveCircuitItem(circuitInput);
                    });

            menu->addAction(actionChangeColor);
            menu->addAction(actionDelete);

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
            QAction* actionChangeColor = new QAction("Change Color", this);
            connect(actionChangeColor, &QAction::triggered,
                    this, [this, circuitOutput] (bool) {
                        emit circuitOutput->closeDialogs();

                        auto* colorDialog = new QColorDialog(this);
                        colorDialog->setOptions(QColorDialog::DontUseNativeDialog);
                        colorDialog->show();
                        colorDialog->setAttribute(Qt::WA_DeleteOnClose);

                        connect(colorDialog, &QColorDialog::colorSelected,
                                circuitOutput, &CircuitOutput::SetColor);
                        connect(circuitOutput, &CircuitOutput::closeDialogs,
                                colorDialog, &QColorDialog::close);
                    });

            QAction* actionDelete = new QAction("Delete", this);
            connect(actionDelete, &QAction::triggered,
                    this, [this, circuitOutput] (bool) {
                        qDebug() << "Action Delete for circuit input invoked, ID ="
                                 << circuitOutput->GetId();

                        RemoveCircuitItem(circuitOutput);
                    });

            menu->addAction(actionChangeColor);
            menu->addAction(actionDelete);

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
            auto eventPos = event->pos();

            QAction* actionSimulate = new QAction("Simulate", this);
            connect(actionSimulate, &QAction::triggered,
                    this, [this, circuitElement] (bool) {
                        if (circuitElement->IsNumberParameterValid())
                        {
                            emit startFunctionalFaultSimulation(circuitElement->GetId());
                        }
                        else
                        {
                            QMessageBox::information(this,
                                tr("Can't simulate on element #")
                                    + QString::number(circuitElement->GetMimeData().orderId),
                                tr("Element has empty or invalid number parameter.\n"
                                   "Please use another one and try again."),
                                QMessageBox::Ok
                            );
                        }
                    });

            QAction* actionChangeColor = new QAction("Change Color", this);
            connect(actionChangeColor, &QAction::triggered,
                    this, [this, circuitElement] (bool) {
                        emit circuitElement->closeDialogs();

                        auto* colorDialog = new QColorDialog(this);
                        colorDialog->setOptions(QColorDialog::DontUseNativeDialog);
                        colorDialog->show();
                        colorDialog->setAttribute(Qt::WA_DeleteOnClose);

                        connect(colorDialog, &QColorDialog::colorSelected,
                                circuitElement, &CircuitElement::SetColor);
                        connect(circuitElement, &CircuitElement::closeDialogs,
                                colorDialog, &QColorDialog::close);
                    });

            QAction* actionChangeSize = new QAction("Change Size", this);
            connect(actionChangeSize, &QAction::triggered,
                    this, [this, circuitElement, eventPos] (bool) {
                        emit circuitElement->closeDialogs();

                        auto* elementSizeChanger = new ElementSizeChanger(circuitElement, this);
                        elementSizeChanger->move(mapToGlobal(eventPos));
                        connect(circuitElement, &CircuitElement::closeDialogs,
                                elementSizeChanger, &ElementSizeChanger::close);
                        // Magic connect: elementSizeChanger passed as 3rd argument
                        // to ensure signal tryToRebookArea will work only while
                        // ElementSizeChanger dialog is opened (and not deleted)
                        connect(circuitElement, &CircuitElement::tryToRebookArea,
                                elementSizeChanger, [this, circuitElement](int previousInputsNumber,
                                                             int previousOutputsNumber,
                                                             EndingPointVector oldEndingPointVector,
                                                             StartingPointVector oldStartingPointVector,
                                                             QRect previousArea){
                                    EndingPointVector endPoints = circuitElement->GetEndPoints();
                                    StartingPointVector startPoints = circuitElement->GetStartPoints();
                                    std::vector<std::pair<QPoint, QPoint>> oldNewPoints;
                                    quint64 connId = 0;
                                    StartingPoint::IdsSet connIdSet;

                                    // Ending point incrementing case
                                    if (endPoints.size() > oldEndingPointVector.size())
                                    {
                                        for (std::size_t i = 0; i < oldEndingPointVector.size(); ++i)
                                        {
                                            auto oldEndingPoint = oldEndingPointVector[i];
                                            auto endPoint = endPoints[i];
                                            oldNewPoints.push_back({oldEndingPoint.connPos,
                                                                    endPoint.connPos});
                                        }
                                        oldNewPoints.push_back({QPoint(),
                                                                endPoints.back().connPos});
                                    }
                                    else
                                    {
                                        // Ending point decrementing case
                                        if (endPoints.size() < oldEndingPointVector.size())
                                        {
                                            connId = oldEndingPointVector.back().connId;
                                        }

                                        for (std::size_t i = 0; i < endPoints.size(); ++i)
                                        {
                                            oldNewPoints.push_back({oldEndingPointVector[i].connPos,
                                                                    endPoints[i].connPos});
                                        }
                                    }

                                    // Starting point incrementing case
                                    if (startPoints.size() > oldStartingPointVector.size())
                                    {
                                        for (std::size_t i = 0; i < oldStartingPointVector.size(); ++i)
                                        {
                                            auto oldStartingPoint = oldStartingPointVector[i];
                                            auto startPoint = startPoints[i];
                                            oldNewPoints.push_back({oldStartingPoint.connPos,
                                                                    startPoint.connPos});
                                        }
                                        oldNewPoints.push_back({QPoint(),
                                                                startPoints.back().connPos});
                                    }
                                    else
                                    {
                                        // Starting point decrementing case
                                        if (startPoints.size() < oldStartingPointVector.size())
                                        {
                                            connIdSet = oldStartingPointVector.back().connIds;
                                        }

                                        for (std::size_t i = 0; i < startPoints.size(); ++i)
                                        {
                                            oldNewPoints.push_back({oldStartingPointVector[i].connPos,
                                                                    startPoints[i].connPos});
                                        }
                                    }

                                    m_areaManager.ClearAndBackupArea(previousArea);

                                    QRect area(circuitElement->pos(), circuitElement->size());
                                    if (m_areaManager.TryBookArea(area, oldNewPoints))
                                    {
                                        // Successfully booked new area
                                        // Decrementing case of inputs size
                                        if (connId)
                                        {
                                            RemoveConnectionById(connId);
                                        }

                                        // Decrementing case of outputs size
                                        if (!connIdSet.empty())
                                        {
                                            for (const auto& connId : connIdSet)
                                            {
                                                RemoveConnectionById(connId);
                                            }
                                        }

                                        circuitElement->SetNumberParameter(0);
                                        emit changeElementItemInputsSize(circuitElement->GetId(),
                                                                         circuitElement->GetEndingConnectors().size());
                                    }
                                    else
                                    {
                                        // Can't book new area
                                        circuitElement->SetInputsNumber(previousInputsNumber);
                                        emit circuitElement->inputsNumber(previousInputsNumber);
                                        circuitElement->SetOutputsNumber(previousOutputsNumber);
                                        emit circuitElement->outputsNumber(previousOutputsNumber);
                                    }

                                    // To redraw connections
                                    update();
                                });
                    });

            QAction* actionDuplicate = new QAction("Duplicate", this);
            connect(actionDuplicate, &QAction::triggered,
                    this, [this, circuitElement, eventPos] (bool) {
                        emit circuitElement->closeDialogs();

                        const auto orderId = GetElementOrderIdHint();
                        auto* dialogDuplicate = new DialogDuplicateElementItem(
                                                circuitElement, orderId, this);
                        dialogDuplicate->move(mapToGlobal(eventPos));

                        connect(this, &CircuitCanvas::setElementOrderIdHint,
                                dialogDuplicate, &DialogDuplicateElementItem::SetElementOrderIdHint);
                        connect(circuitElement, &CircuitElement::closeDialogs,
                                dialogDuplicate, &DialogDuplicateElementItem::close);

            });

            QAction* actionDelete = new QAction("Delete", this);
            connect(actionDelete, &QAction::triggered,
                    this, [this, circuitElement] (bool) {
                        qDebug() << "Action Delete for circuit input invoked, ID ="
                                 << circuitElement->GetId();

                        RemoveCircuitItem(circuitElement);
            });

            menu->addAction(actionSimulate);
            menu->addAction(actionChangeColor);
            menu->addAction(actionChangeSize);
            menu->addAction(actionDuplicate);
            menu->addAction(actionDelete);

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

    if (auto* circuitInput = qobject_cast<CircuitInput*>(item); circuitInput)
    {
        const auto mimeData = circuitInput->GetMimeData();
        m_idHandler.RemoveInputOrderId(mimeData.orderId);

        const auto& connIdSet = mimeData.startPoint.connIds;
        for (const auto& connId : connIdSet)
        {
            RemoveConnectionById(connId);
        }
    }
    else if (auto* circuitOutput = qobject_cast<CircuitOutput*>(item); circuitOutput)
    {
        const auto mimeData = circuitOutput->GetMimeData();
        m_idHandler.RemoveOutputOrderId(mimeData.orderId);

        const auto connId = mimeData.connId;
        RemoveConnectionById(connId);
    }
    else if (auto* circuitElement = qobject_cast<CircuitElement*>(item); circuitElement)
    {
        const auto mimeData = circuitElement->GetMimeData();
        m_idHandler.RemoveElementOrderId(mimeData.orderId);

        const auto& startPoints = mimeData.startingPointVector;
        for (const auto& startPoint : startPoints)
        {
            const auto& connIdSet = startPoint.connIds;
            for (const auto& connId : connIdSet)
            {
                RemoveConnectionById(connId);
            }
        }

        const auto& endPoints = mimeData.endingPointVector;
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

    QRect area(item->pos(), item->size());
    m_areaManager.ClearArea(area);

    item->close();

    update();
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

void CircuitCanvas::SaveCircuitItem(BaseCircuitItem* item, json& metaItems)
{
    if (!item)
    {
        return;
    }

    json itemMeta;

    if (auto* circuitInput = qobject_cast<CircuitInput*>(item); circuitInput)
    {
        const auto mimeData = circuitInput->GetMimeData();

        itemMeta["type"] = circuitInput->GetItemType();
        itemMeta["id"] = mimeData.id;
        itemMeta["orderId"] = mimeData.orderId;
        itemMeta["width"] = mimeData.itemSize.width();
        itemMeta["height"] = mimeData.itemSize.height();
        itemMeta["pos"]["x"] = mimeData.itemPosition.x();
        itemMeta["pos"]["y"] = mimeData.itemPosition.y();
        itemMeta["color"] =
            static_cast<quint64>(mimeData.color.rgba64());

        auto& metaStartPoint = itemMeta["startPoint"];
        const auto& startPoint = mimeData.startPoint;
        metaStartPoint["connPos"]["x"] = startPoint.connPos.x();
        metaStartPoint["connPos"]["y"] = startPoint.connPos.y();
    }
    else if (auto* circuitOutput = qobject_cast<CircuitOutput*>(item); circuitOutput)
    {
        const auto mimeData = circuitOutput->GetMimeData();

        itemMeta["type"] = circuitOutput->GetItemType();
        itemMeta["id"] = mimeData.id;
        itemMeta["orderId"] = mimeData.orderId;
        itemMeta["width"] = mimeData.itemSize.width();
        itemMeta["height"] = mimeData.itemSize.height();
        itemMeta["pos"]["x"] = mimeData.itemPosition.x();
        itemMeta["pos"]["y"] = mimeData.itemPosition.y();
        itemMeta["color"] =
            static_cast<quint64>(mimeData.color.rgba64());

        auto& metaEndPoint = itemMeta["endPoint"];
        const auto& endPoint = mimeData.endPoint;
        metaEndPoint["connPos"]["x"] = endPoint.connPos.x();
        metaEndPoint["connPos"]["y"] = endPoint.connPos.y();
    }
    else if (auto* circuitElement = qobject_cast<CircuitElement*>(item); circuitElement)
    {
        const auto mimeData = circuitElement->GetMimeData();

        itemMeta["type"] = circuitElement->GetItemType();
        itemMeta["id"] = mimeData.id;
        itemMeta["orderId"] = mimeData.orderId;
        itemMeta["width"] = mimeData.itemSize.width();
        itemMeta["height"] = mimeData.itemSize.height();
        itemMeta["pos"]["x"] = mimeData.itemPosition.x();
        itemMeta["pos"]["y"] = mimeData.itemPosition.y();
        itemMeta["color"] =
            static_cast<quint64>(mimeData.color.rgba64());

        itemMeta["numberParam"] = mimeData.numberParam;
        itemMeta["isNotationBinary"] = mimeData.isNotationBinary;

        auto& metaEndPoints = itemMeta["endPoints"];
        for (const auto& [connPos, _] : mimeData.endingPointVector)
        {
            json endPoint;
            endPoint["connPos"]["x"] = connPos.x();
            endPoint["connPos"]["y"] = connPos.y();

            metaEndPoints.push_back(endPoint);
        }

        auto& metaStartPoints = itemMeta["startPoints"];
        for (const auto& [connPos, _] : mimeData.startingPointVector)
        {
            json startPoint;
            startPoint["connPos"]["x"] = connPos.x();
            startPoint["connPos"]["y"] = connPos.y();

            metaStartPoints.push_back(startPoint);
        }
    }
    else
    {
        throw std::runtime_error("ItemType is unknown, type = "
                                 + std::to_string(item->GetItemType()));
        return;
    }

    qDebug() << "Saving input item:" << itemMeta.dump().c_str();

    metaItems.push_back(itemMeta);
}

void CircuitCanvas::SaveItemConnections(json& metaConnections)
{
    for (auto&& [_, conn] : m_areaManager.GetConnections())
    {
        auto&& [startId, endId, line] = conn;
        json connection;

        connection["startId"] = startId;
        connection["endId"] = endId;
        connection["startPos"]["x"] = line.p1().x();
        connection["startPos"]["y"] = line.p1().y();
        connection["endPos"]["x"] = line.p2().x();
        connection["endPos"]["y"] = line.p2().y();

        metaConnections.push_back(connection);
    }
}

void CircuitCanvas::ConstructItemsFromJson(const json& metaRoot)
{
    qDebug() << "ConstructItemsFromJson";

    const auto& itemsArray = metaRoot.at("items");
    if (!itemsArray.is_null() && itemsArray.is_array())
    {
        qDebug() << "Items array is not empty, size =" << itemsArray.size();
        for (const auto& item : itemsArray)
        {
            auto itemType = item.at("type").template get<int>();
            switch (itemType)
            {
                case ItemType::Input:
                {
                    ConstructInputItemFromJson(item);

                    break;
                }
                case ItemType::Output:
                {
                    ConstructOutputItemFromJson(item);

                    break;
                }
                case ItemType::Element:
                {
                    ConstructElementItemFromJson(item);

                    break;
                }
                default:
                {
                    continue;
                }
            }
        }
    }

    const auto& connectionsArray = metaRoot.at("connections");
    if (!connectionsArray.is_null() && connectionsArray.is_array())
    {
        qDebug() << "Connections array is not empty, size ="
                 << connectionsArray.size();
        for (const auto& connection : connectionsArray)
        {
            ConstructConnectionFromJson(connection);
        }
    }
}

void CircuitCanvas::ConstructInputItemFromJson(const json& item)
{
    const auto& pos = item.at("pos");
    auto itemX = pos.at("x").template get<int>();
    auto itemY = pos.at("y").template get<int>();
    QPoint itemPosition(itemX, itemY);

    auto width = item.at("width").template get<int>();
    auto height = item.at("height").template get<int>();
    QSize itemSize(width, height);

    QRect area(itemPosition, itemSize);

    std::vector<std::pair<QPoint, QPoint>> emptyVector;

    if (!m_areaManager.TryBookArea(area, emptyVector))
    {
        return;
    }

    CircuitInputMimeData mimeData;

    mimeData.id = item.at("id").template get<quint64>();
    mimeData.orderId = item.at("orderId").template get<int>();

    auto orderIdInserted = m_idHandler.NewInputOrderId(mimeData.orderId);
    auto uidInserted = m_idHandler.InsertUid(mimeData.id);

    if (!orderIdInserted || !uidInserted)
    {
        return;
    }

    emit addNewInputItem(mimeData.id);

    quint64 rgba64Color = item.at("color").template get<quint64>();
    mimeData.color = QRgba64::fromRgba64(rgba64Color);

    const auto& connPos = item.at("startPoint").at("connPos");
    auto connX = connPos.at("x").template get<int>();
    auto connY = connPos.at("y").template get<int>();
    mimeData.startPoint.connPos = {connX, connY};

    auto* circuitItem = new CircuitInput(mimeData, this);
    circuitItem->move(itemPosition);
}

void CircuitCanvas::ConstructOutputItemFromJson(const json& item)
{
    const auto& pos = item.at("pos");
    auto itemX = pos.at("x").template get<int>();
    auto itemY = pos.at("y").template get<int>();
    QPoint itemPosition(itemX, itemY);

    auto width = item.at("width").template get<int>();
    auto height = item.at("height").template get<int>();
    QSize itemSize(width, height);

    QRect area(itemPosition, itemSize);

    std::vector<std::pair<QPoint, QPoint>> emptyVector;

    if (!m_areaManager.TryBookArea(area, emptyVector))
    {
        return;
    }

    CircuitOutputMimeData mimeData;

    mimeData.id = item.at("id").template get<quint64>();
    mimeData.orderId = item.at("orderId").template get<int>();

    auto orderIdInserted = m_idHandler.NewOutputOrderId(mimeData.orderId);
    auto uidInserted = m_idHandler.InsertUid(mimeData.id);

    if (!orderIdInserted || !uidInserted)
    {
        return;
    }

    emit addNewOutputItem(mimeData.id);

    quint64 rgba64Color = item.at("color").template get<quint64>();
    mimeData.color = QRgba64::fromRgba64(rgba64Color);

    const auto& connPos = item.at("endPoint").at("connPos");
    auto connX = connPos.at("x").template get<int>();
    auto connY = connPos.at("y").template get<int>();
    mimeData.endPoint.connPos = {connX, connY};

    auto* circuitItem = new CircuitOutput(mimeData, this);
    circuitItem->move(itemPosition);
}

void CircuitCanvas::ConstructElementItemFromJson(const json& item)
{
    const auto& pos = item.at("pos");
    auto itemX = pos.at("x").template get<int>();
    auto itemY = pos.at("y").template get<int>();
    QPoint itemPosition(itemX, itemY);

    auto width = item.at("width").template get<int>();
    auto height = item.at("height").template get<int>();
    QSize itemSize(width, height);

    QRect area(itemPosition, itemSize);

    std::vector<std::pair<QPoint, QPoint>> emptyVector;

    if (!m_areaManager.TryBookArea(area, emptyVector))
    {
        return;
    }

    CircuitElementMimeData mimeData;

    mimeData.id = item.at("id").template get<quint64>();
    mimeData.orderId = item.at("orderId").template get<int>();

    auto orderIdInserted = m_idHandler.NewElementOrderId(mimeData.orderId);
    auto uidInserted = m_idHandler.InsertUid(mimeData.id);

    if (!orderIdInserted || !uidInserted)
    {
        return;
    }

    emit addNewElementItem(mimeData.id, mimeData.endingPointVector.size());

    quint64 rgba64Color = item.at("color").template get<quint64>();
    mimeData.color = QRgba64::fromRgba64(rgba64Color);

    mimeData.numberParam = item.at("numberParam").template get<int>();
    mimeData.isNotationBinary = item.at("isNotationBinary").template get<bool>();

    const auto& endPoints = item.at("endPoints");
    if (!endPoints.is_null() && endPoints.is_array())
    {
        for (const auto& endPoint : endPoints)
        {
            const auto& connPos = endPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            EndingPoint point;
            point.connPos = {connX, connY};
            mimeData.endingPointVector.push_back(point);
        }
    }

    const auto& startPoints = item.at("startPoints");
    if (!startPoints.is_null() && startPoints.is_array())
    {
        for (const auto& startPoint : startPoints)
        {
            const auto& connPos = startPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            StartingPoint point;
            point.connPos = {connX, connY};
            mimeData.startingPointVector.push_back(point);
        }
    }

    auto* circuitItem = new CircuitElement(mimeData, this);
    circuitItem->move(itemPosition);
}

void CircuitCanvas::ConstructConnectionFromJson(const json& connection)
{
    const auto startId = connection.at("startId").template get<quint64>();
    const auto endId = connection.at("endId").template get<quint64>();

    const auto startPos = connection.at("startPos");
    auto startX = startPos.at("x").template get<int>();
    auto startY = startPos.at("y").template get<int>();
    QPoint startPosPoint = {startX, startY};

    const auto endPos = connection.at("endPos");
    auto endX = endPos.at("x").template get<int>();
    auto endY = endPos.at("y").template get<int>();
    QPoint endPosPoint = {endX, endY};

    const auto positions = QLine(startPosPoint, endPosPoint);

    InsertConnection(startId, endId, positions);
}
