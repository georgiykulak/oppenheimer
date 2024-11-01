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
        dataStream >> pixmap >> endingPosition >> offset;

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
        dataStream >> pixmap >> endingPosition >> offset;

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

            // TODO: Add constructor for CircuitInputMimeData
            CircuitInput *cInput = new CircuitInput(mimeData.startPoint, this);
            cInput->SetId(mimeData.id);
            cInput->SetOrderId(mimeData.orderId);
            cInput->SetValue(mimeData.value);
            cInput->SetPixmap(mimeData.pixmap);
            cInput->SetSize(mimeData.itemSize);
            cInput->SetColor(mimeData.color);
            cInput->move(mimeData.itemPosition);
            cInput->update();
            cInput->show();
            cInput->setAttribute(Qt::WA_DeleteOnClose);

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

            CircuitOutput *cOutput = new CircuitOutput(mimeData.endPoint, this);
            cOutput->SetId(mimeData.id);
            cOutput->SetOrderId(mimeData.orderId);
            cOutput->SetValue(mimeData.value);
            cOutput->SetPixmap(mimeData.pixmap);
            cOutput->SetSize(mimeData.itemSize);
            cOutput->SetColor(mimeData.color);
            cOutput->move(mimeData.itemPosition);
            cOutput->update();
            cOutput->show();
            cOutput->setAttribute(Qt::WA_DeleteOnClose);

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
                emit addNewElementItem(mimeData.id, mimeData.endingPointVectorSize);

                emit setElementOrderIdHint(m_idHandler.GetLastElementOrderId());
            }

            CircuitElement *cElement = new CircuitElement(mimeData.endingPointVector,
                                                          mimeData.startingPointVector,
                                                          this);
            cElement->SetId(mimeData.id);
            cElement->SetOrderId(mimeData.orderId);
            cElement->SetNumberParameter(mimeData.numberParam);
            cElement->SetValue(mimeData.value);
            cElement->SetSize(mimeData.itemSize);
            cElement->SetColor(mimeData.color);
            cElement->SetNotation(mimeData.isNotationBinary);
            cElement->move(mimeData.itemPosition);
            cElement->update();
            cElement->show();
            cElement->setAttribute(Qt::WA_DeleteOnClose);

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
        QPoint endingPosition;
        QPoint offset;
        dataStream >> pixmap >> endingPosition >> offset;

        m_currentConnectingLine = {};
        QWidget* child = childAt(event->pos());
        if (child)
        {
            StartingConnector* startingConnector =
                qobject_cast<StartingConnector*>(child);
            if (startingConnector)
            {
                qDebug() << "Create new connection HERE (end -> start)";

                const auto connId = m_areaManager.AddConnection(
                    QLine(startingConnector->GetStartPoint().connPos,
                          endingPosition + offset)
                    );

                if (connId)
                {
                    startingConnector->InsertConnectionId(connId);
                    emit setEndingInitiatorConnectionId(connId);
                }

                update();
                event->setDropAction(Qt::CopyAction);
                event->accept();

                return;
            }
        }

        update();
        event->ignore();
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

        m_currentConnectingLine = {};
        QWidget* child = childAt(event->pos());
        if (child)
        {
            EndingConnector* endingConnector =
                qobject_cast<EndingConnector*>(child);
            if (endingConnector)
            {
                if (endingConnector->IsConnected())
                {
                    qDebug() << "Can't connect: busy by connection, id ="
                             << endingConnector->GetConnectionId();

                    update();
                    event->ignore();
                    return;
                }

                qDebug() << "Create new connection HERE (start -> end)";

                const auto connId = m_areaManager.AddConnection(
                    QLine(startingPosition + offset,
                          endingConnector->GetEndPoint().connPos)
                    );

                if (connId)
                {
                    endingConnector->SetConnectionId(connId);
                    emit insertStartingInitiatorConnectionId(connId);
                }

                update();
                event->setDropAction(Qt::CopyAction);
                event->accept();

                return;
            }
        }

        update();
        event->ignore();
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
        return;

    CircuitInput* circuitInput = qobject_cast<CircuitInput*>(child);
    if (circuitInput)
    {
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

            QRect previousArea(circuitInput->pos(), itemSize);
            m_areaManager.ClearAndBackupArea(previousArea);

            if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
            {
                circuitInput->close();
            }
            else
            {
                circuitInput->show();
                circuitInput->SetPixmap(pixmap);
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
                        emit circuitInput->closeDialogs();

                        qDebug() << "Action Delete for circuit input invoked, ID ="
                                 << circuitInput->GetId();

                        const auto uid = circuitInput->GetId();
                        emit removeItem(uid);
                        m_idHandler.RemoveUid(circuitInput->GetId());
                        m_idHandler.RemoveInputOrderId(circuitInput->GetOrderId());

                        const auto connIdSet = circuitInput->GetStartPoint().connIds;
                        for (const auto& connId : connIdSet)
                        {
                            RemoveConnectionById(connId);
                        }

                        QRect area(circuitInput->pos(), circuitInput->GetSize());
                        m_areaManager.ClearArea(area);

                        circuitInput->close();

                        update();
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

            QRect previousArea(circuitOutput->pos(), itemSize);
            m_areaManager.ClearAndBackupArea(previousArea);

            if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
            {
                circuitOutput->close();
            }
            else
            {
                circuitOutput->show();
                circuitOutput->SetPixmap(pixmap);
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
                        emit circuitOutput->closeDialogs();

                        // TODO: Add deleting code here
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
                dataStream
                    << endPoint.connPos
                    << QPoint(event->pos() - endPoint.connPos)
                    << endPoint.connId;
            }

            dataStream << (unsigned int)(startPoints.size());
            for (const auto& startPoint : startPoints)
            {
                dataStream
                    << startPoint.connPos
                    << QPoint(event->pos() - startPoint.connPos);

                dataStream << (unsigned int)(startPoint.connIds.size());
                for (const quint64 connId : startPoint.connIds)
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

            QRect previousArea(circuitElement->pos(), itemSize);
            m_areaManager.ClearAndBackupArea(previousArea);

            if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
            {
                circuitElement->close();
            }
            else
            {
                circuitElement->show();
                circuitElement->SetPixmap(pixmap);
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
                                    + QString::number(circuitElement->GetOrderId()),
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

                                    QRect area(circuitElement->pos(), circuitElement->GetSize());
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
                                                                         circuitElement->GetEndPoints().size());
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
                        emit circuitElement->closeDialogs();

                        // TODO: Add deleting code here
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
            QPoint offset = {};

            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << pixmap << endingPosition << offset;

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

            connect(this, &CircuitCanvas::setEndingInitiatorConnectionId,
                    endingConnector, &EndingConnector::SetConnectionId);

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
            QPoint offset = {};

            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << pixmap << startingPosition << offset;

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

            connect(this, &CircuitCanvas::insertStartingInitiatorConnectionId,
                    startingConnector, &StartingConnector::InsertConnectionId);

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
        CircuitInput* circuitInput = qobject_cast<CircuitInput*>(obj);
        if (circuitInput)
        {
            const auto& connIdSet = circuitInput->GetStartPoint().connIds;
            if (connIdSet.contains(connId))
            {
                qDebug() << "Found input item containing connection ID =" << connId
                         << "item ID =" << circuitInput->GetId();
                circuitInput->RemoveConnectionId(connId);
            }
        }

        CircuitOutput* circuitOutput = qobject_cast<CircuitOutput*>(obj);
        if (circuitOutput)
        {
            if (circuitOutput->GetEndPoint().connId == connId)
            {
                qDebug() << "Found output item containing connection ID =" << connId
                         << "item ID =" << circuitOutput->GetId();
                circuitOutput->RemoveConnectionId(connId);
            }
        }

        CircuitElement* circuitElement = qobject_cast<CircuitElement*>(obj);
        if (circuitElement)
        {
            const auto& startPoints = circuitElement->GetStartPoints();
            for (const auto& startPoint : startPoints)
            {
                if (startPoint.connIds.contains(connId))
                {
                    qDebug() << "Found element item (starting side) containing connection ID ="
                             << connId << "item ID =" << circuitElement->GetId();
                    circuitElement->RemoveConnectionId(connId);
                    break;
                }
            }

            const auto& endPoints = circuitElement->GetEndPoints();
            for (const auto& endPoint : endPoints)
            {
                if (endPoint.connId == connId)
                {
                    qDebug() << "Found element item (ending side) containing connection ID ="
                             << connId << "item ID =" << circuitElement->GetId();
                    circuitElement->RemoveConnectionId(connId);
                    break;
                }
            }
        }
    }
}
