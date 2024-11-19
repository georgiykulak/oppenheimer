#include "CircuitElement.hpp"
#include "connectors/EndingConnector.hpp"
#include "connectors/StartingConnector.hpp"
#include "widgets/LogicVectorEdit.hpp"
#include "dialogues/ElementSizeChanger.hpp"
#include "dialogues/DialogDuplicateElementItem.hpp"
#include "circuits/ItemRegistry.hpp"
#include "Config.hpp"

#include <QPainter>
#include <QPushButton>
#include <QMenu>
#include <QMessageBox>

CircuitElement::CircuitElement(const CircuitElementMimeData& mimeData,
                               QWidget *parent,
                               bool numParamEnabled)
    : BaseCircuitItem{parent}
{
    m_color = Qt::lightGray;

    m_minimumHeight = 110;
    m_minimumYShift = 55;
    QSize size;
    if (mimeData.itemSize.isValid())
    {
        size = mimeData.itemSize;
    }
    else
    {
        const auto realHeight = m_minimumHeight + m_offsetBetweenConnection *
                           (std::max(mimeData.endingPoints.size(),
                                     mimeData.startingPoints.size()) - 1);
        size = QSize(110, realHeight);
    }
    setFixedSize(size);
    m_pixmap = QPixmap(this->size());
    m_pixmap.fill(QColor(Qt::transparent));

    QPixmap pixmap(QSize(12, 12)); // Connector size
    pixmap.fill(QColor(Qt::transparent));
    QPainter painter(&pixmap);

    //////////////////////////////////////////////////////////////////////////////////////////

    int i = 0;
    for (const auto& endPoint : mimeData.endingPoints)
    {
        const int yShift = m_minimumYShift + m_offsetBetweenConnection * i++;
        QPoint positionOffset(3, yShift - 4);
        EndingConnector* endingConnector =
            new EndingConnector(endPoint, positionOffset, this);

        endingConnector->move(positionOffset);
        endingConnector->update();
        endingConnector->show();
        endingConnector->setAttribute(Qt::WA_DeleteOnClose);

        m_endingConnectors.push_back(endingConnector);
    }

    i = 0;
    for (const auto& startPoint : mimeData.startingPoints)
    {
        const int yShift = m_minimumYShift + m_offsetBetweenConnection * i++;
        QPoint positionOffset(95, yShift - 4);
        StartingConnector* startingConnector =
            new StartingConnector(startPoint, positionOffset, this);

        startingConnector->move(positionOffset);
        startingConnector->update();
        startingConnector->show();
        startingConnector->setAttribute(Qt::WA_DeleteOnClose);

        m_startingConnectors.push_back(startingConnector);
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    m_textField = new LogicVectorEdit(this);
    m_textField->setMinimumSize(60, 30);
    m_textField->setMaximumSize(60, 30);
    m_textField->move(25, 45);
    m_textField->show();
    m_textField->setEnabled(numParamEnabled);

    const auto vectorSize = 1 << mimeData.endingPoints.size(); // 2 ^ N
    m_textField->setMaximumDigitCount(vectorSize);
    m_textField->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_textField, &LogicVectorEdit::numberChangedAndValid,
            this, [this](int number)
            {
                m_numberParam = number;
                emit setNumberParameterToElementItem(
                    GetId(), m_numberParam
                );
            });
    connect(m_textField, &LogicVectorEdit::setNumberValidity,
            this, [this](bool isValid)
            {
                m_numberParameterIsValid = isValid;
            });
    connect(m_textField, &LogicVectorEdit::textRowsCountChanged,
            this, [this]()
            {
                auto lePos = m_textField->pos();
                m_notationSwitchButton->move(lePos.x(),
                                             lePos.y() + m_textField->height());

                update();
            });

    m_notationSwitchButton = new QPushButton("bin", this);
    m_notationSwitchButton->setMinimumSize(30, 15);
    m_notationSwitchButton->setMaximumSize(30, 15);
    auto lePos = m_textField->pos();
    m_notationSwitchButton->move(lePos.x(), lePos.y() + m_textField->height());
    m_textField->show();
    m_textField->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_notationSwitchButton, &QAbstractButton::released,
            this, [this](){
                if (m_textField->IsNotationBinary())
                {
                    m_notationSwitchButton->setText("dec");
                    m_textField->setNotation(false);
                }
                else
                {
                    m_notationSwitchButton->setText("bin");
                    m_textField->setNotation(true);
                }
            });


    //////////////////////////////////////////////////////////////////////////////////////////

    m_id = mimeData.id;
    m_orderId = mimeData.orderId;
    m_outputValue = mimeData.value;
    if (mimeData.color.isValid())
    {
        m_color = mimeData.color;
    }

    m_numberParam = mimeData.numberParam;
    m_textField->setNumber(m_numberParam);
    m_notationSwitchButton->setText(mimeData.isNotationBinary ? "bin" : "dec");
    m_textField->setNotation(mimeData.isNotationBinary);

    CircuitElement::DrawToPixmap();
    show();
    setAttribute(Qt::WA_DeleteOnClose);
}

void CircuitElement::ConstructCircuitElementFromJson(const RequiredItemMeta& reqMeta,
                                                     const json& itemMeta,
                                                     QWidget* canvas)
{
    CircuitElementMimeData mimeData;
    mimeData.endingPoints = reqMeta.endingPoints;
    mimeData.startingPoints = reqMeta.startingPoints;
    mimeData.color = reqMeta.color;
    mimeData.itemSize = reqMeta.itemSize;
    mimeData.itemPosition = reqMeta.itemPosition;
    mimeData.id = reqMeta.id;
    mimeData.orderId = reqMeta.orderId;

    mimeData.numberParam = itemMeta.at("numberParam").template get<int>();
    mimeData.isNotationBinary =
        itemMeta.at("isNotationBinary").template get<bool>();

    auto* item = new CircuitElement(mimeData, canvas);
    item->move(mimeData.itemPosition);
}

void CircuitElement::ConstructCircuitElementFromStream(const BaseCircuitItemMimeData& baseMimeData,
                                                       QDataStream& additionalData,
                                                       ItemRegistry* itemRegistry)
{
    auto* parentWidget = qobject_cast<QWidget*>(itemRegistry->parent());
    if (!parentWidget)
    {
        return;
    }

    CircuitElementMimeData mimeData;
    mimeData.endingPoints = baseMimeData.endingPoints;
    mimeData.startingPoints = baseMimeData.startingPoints;
    mimeData.color = baseMimeData.color;
    mimeData.itemSize = baseMimeData.itemSize;
    mimeData.itemPosition = baseMimeData.itemPosition;
    mimeData.id = baseMimeData.id;
    mimeData.orderId = baseMimeData.orderId;
    //mimeData.readMimeData(additionalData);

    auto* item = new CircuitElement(mimeData, parentWidget);
    item->move(mimeData.itemPosition);

    connect(item, &BaseCircuitItem::removeCircuitItem,
            itemRegistry, &ItemRegistry::removeCircuitItem);

    connect(item, &CircuitElement::setNumberParameterToElementItem,
            itemRegistry, &ItemRegistry::setNumberParameterToElementItem);

    connect(item, &CircuitElement::startFunctionalFaultSimulation,
            itemRegistry, &ItemRegistry::startFunctionalFaultSimulation);

    connect(item, &CircuitElement::askOrderIdHint,
            itemRegistry, &ItemRegistry::askOrderIdHint);

    connect(itemRegistry, &ItemRegistry::setOrderIdHintForDuplicate,
            item, &CircuitElement::setOrderIdHintForDuplicate);

    connect(item, &CircuitElement::tryToRebookArea,
            itemRegistry, &ItemRegistry::tryToRebookArea);
}

void CircuitElement::DrawToPixmap()
{
    QPainter painter(&m_pixmap);

    QPen mPen;
    mPen.setColor(m_color);
    painter.setPen(mPen);
    painter.setBrush(m_color);
    int wBig = 90;
    int hBig = height();
    const int borderWidth = 2;
    painter.drawRoundedRect(10, borderWidth - 1, wBig, hBig - borderWidth, 10, 10, Qt::AbsoluteSize);

#ifdef DRAW_ELEMENT_ITEM_BORDERS
    mPen.setWidth(borderWidth);
    mPen.setColor(Qt::darkGray);
    painter.setPen(mPen);
    painter.drawRoundedRect(10, borderWidth - 1, wBig, hBig - borderWidth, 10, 10, Qt::AbsoluteSize);
#endif

    int xStartText = 25;
    int yStartText = 45;
    int wSmall = 60;
    int hSmall = m_textField->height();
    mPen.setWidth(2);
    mPen.setColor(Qt::darkGray);
    painter.setPen(mPen);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(xStartText, yStartText, wSmall, hSmall, 3, 3, Qt::AbsoluteSize);

    mPen.setColor(Qt::gray);
    painter.setPen(mPen);
    painter.setFont(QFont("Arial"));
    QString strParam = "...";
    painter.drawText(QRect(xStartText, yStartText, wSmall, hSmall), Qt::AlignCenter, strParam);

    mPen.setColor(Qt::black);
    painter.setPen(mPen);
    painter.setFont(QFont("Arial"));
    QString strVal;
    strVal.setNum(m_outputValue);
    painter.drawText(QRect(80, 10, 10, 15), Qt::AlignCenter, strVal);

    mPen.setColor(Qt::black);
    painter.setPen(mPen);
    painter.setFont(QFont("Arial"));
    QString strNum;
    strNum.setNum(m_orderId);
    painter.drawText(QRect(30, 10, 50, 30), Qt::AlignCenter, strNum);

    for (auto* endingConnector : m_endingConnectors)
    {
        EndingConnector::DrawConnectorToPixmap(painter,
                                endingConnector->GetPositionOffset());
    }

    for (auto* startingConnector : m_startingConnectors)
    {
        StartingConnector::DrawConnectorToPixmap(painter,
                                startingConnector->GetPositionOffset());
    }
}

void CircuitElement::SetNumberParameter(int numberParam)
{
    m_numberParam = numberParam;
    m_textField->setNumber(numberParam);
}

void CircuitElement::SetValue(bool value)
{
    m_outputValue = value;
}

json CircuitElement::GetJsonMeta() const
{
    auto elementMeta = BaseCircuitItem::GetJsonMeta();

    elementMeta["numberParam"] = m_numberParam;
    elementMeta["isNotationBinary"] = m_textField->IsNotationBinary();

    return elementMeta;
}

void CircuitElement::SetInputsNumber(int size)
{
    if (size <= 0)
        return;

    const std::size_t number = size;

    const auto vectorSize = 1 << number; // 2 ^ N
    m_textField->setMaximumDigitCount(vectorSize);
    m_textField->setNumber(m_numberParam);
    const auto calculatedHeight =
        25 + m_textField->height() + m_notationSwitchButton->height() + 25;

    if (number > m_endingConnectors.size())
    {
        int newHeight = m_minimumHeight + (number - 1) * m_offsetBetweenConnection;
        if (calculatedHeight > newHeight)
        {
            newHeight = calculatedHeight;
        }

        if (newHeight > height())
        {
            QSize newWidgetSize(width(), newHeight);
            setFixedSize(newWidgetSize);

            m_pixmap = QPixmap(this->size());
            m_pixmap.fill(QColor(Qt::transparent));
            DrawToPixmap();
        }

        QPixmap pixmap(QSize(12, 12)); // Connector size
        pixmap.fill(QColor(Qt::transparent));

        for (std::size_t i = m_endingConnectors.size(); i < number; ++i)
        {
            const auto& prevEndPoint = m_endingConnectors[i - 1]->GetEndPoint();
            QPoint shift = prevEndPoint.connPos + QPoint(0, m_offsetBetweenConnection);
            EndingPoint endPoint = {shift, 0};

            const int yShift = m_minimumYShift + m_offsetBetweenConnection * i;
            QPoint positionOffset(3, yShift - 4);
            EndingConnector* endingConnector =
                new EndingConnector(endPoint, positionOffset, this);

            endingConnector->move(positionOffset);
            endingConnector->update();
            endingConnector->show();
            endingConnector->setAttribute(Qt::WA_DeleteOnClose);

            m_endingConnectors.push_back(endingConnector);
        }
    }
    else if (number < m_endingConnectors.size())
    {
        for (auto i = m_endingConnectors.size() - 1; i != number - 1; --i)
        {
            m_endingConnectors[i]->close();
        }

        m_endingConnectors.resize(number);

        int newHeight = m_minimumHeight + (number - 1) * m_offsetBetweenConnection;
        if (calculatedHeight > newHeight)
        {
            newHeight = calculatedHeight;
        }

        if (newHeight < height() && number >= m_startingConnectors.size())
        {
            QSize newWidgetSize(width(), newHeight);
            setFixedSize(newWidgetSize);
        }

        m_pixmap = QPixmap(this->size());
        m_pixmap.fill(QColor(Qt::transparent));
        DrawToPixmap();
    }

    update();
}

void CircuitElement::SetInputsNumberAndRebook(int size)
{
    const int currentInputsNumber = static_cast<int>(m_endingConnectors.size());
    if (size == currentInputsNumber)
    {
        return;
    }

    // Store current state
    EndingPointVector currentEndingPointVector;
    for (const auto* endingConnector : m_endingConnectors)
    {
        currentEndingPointVector.push_back(endingConnector->GetEndPoint());
    }

    StartingPointVector currentStartingPointVector;
    for (const auto* startingConnector : m_startingConnectors)
    {
        currentStartingPointVector.push_back(startingConnector->GetStartPoint());
    }

    QRect currentArea(pos(), this->size());

    // Try upcoming changes
    SetInputsNumber(size);

    emit checkAndTryToRebookArea(currentInputsNumber,
                                       m_startingConnectors.size(),
                                       currentEndingPointVector,
                                       currentStartingPointVector,
                                       currentArea);
}

void CircuitElement::SetOutputsNumber(int size)
{
    if (size <= 0)
        return;

    const std::size_t number = size;

    if (number > m_startingConnectors.size())
    {
        const int newHeight = m_minimumHeight + (number - 1) * m_offsetBetweenConnection;
        if (newHeight > height())
        {
            QSize newWidgetSize(width(), newHeight);
            setFixedSize(newWidgetSize);

            m_pixmap = QPixmap(this->size());
            m_pixmap.fill(QColor(Qt::transparent));
            DrawToPixmap();
        }

        QPixmap pixmap(QSize(12, 12)); // Connector size
        pixmap.fill(QColor(Qt::transparent));

        for (std::size_t i = m_startingConnectors.size(); i < number; ++i)
        {
            const auto& prevStartPoint = m_startingConnectors[i - 1]->GetStartPoint();
            QPoint shift = prevStartPoint.connPos + QPoint(0, m_offsetBetweenConnection);
            StartingPoint startPoint = {shift, StartingPoint::IdsSet()};

            const int yShift = m_minimumYShift + m_offsetBetweenConnection * i;
            QPoint positionOffset(95, yShift - 4);
            StartingConnector* startingConnector =
                new StartingConnector(startPoint, positionOffset, this);

            startingConnector->move(positionOffset);
            startingConnector->update();
            startingConnector->show();
            startingConnector->setAttribute(Qt::WA_DeleteOnClose);

            m_startingConnectors.push_back(startingConnector);
        }
    }
    else if (number < m_startingConnectors.size())
    {
        for (auto i = m_startingConnectors.size() - 1; i != number - 1; --i)
        {
            m_startingConnectors[i]->close();
        }

        m_startingConnectors.resize(number);

        const int newHeight = m_minimumHeight + (number - 1) * m_offsetBetweenConnection;
        if (newHeight < height() && number >= m_endingConnectors.size())
        {
            QSize newWidgetSize(width(), newHeight);
            setFixedSize(newWidgetSize);
        }

        m_pixmap = QPixmap(this->size());
        m_pixmap.fill(QColor(Qt::transparent));
        DrawToPixmap();
    }

    update();
}

void CircuitElement::SetOutputsNumberAndRebook(int size)
{
    const int currentOutputsNumber = static_cast<int>(m_startingConnectors.size());
    if (size == currentOutputsNumber)
    {
        return;
    }

    // Store current state
    EndingPointVector currentEndingPointVector;
    for (const auto* endingConnector : m_endingConnectors)
    {
        currentEndingPointVector.push_back(endingConnector->GetEndPoint());
    }

    StartingPointVector currentStartingPointVector;
    for (const auto* startingConnector : m_startingConnectors)
    {
        currentStartingPointVector.push_back(startingConnector->GetStartPoint());
    }

    QRect currentArea(pos(), this->size());

    // Try upcoming changes
    SetOutputsNumber(size);

    emit checkAndTryToRebookArea(m_endingConnectors.size(),
                                       currentOutputsNumber,
                                       currentEndingPointVector,
                                       currentStartingPointVector,
                                       currentArea);
}

EndingPointVector CircuitElement::GetEndPoints() const
{
    EndingPointVector endingPoints;

    for (const auto* endingConnector : m_endingConnectors)
    {
        endingPoints.push_back(endingConnector->GetEndPoint());
    }

    return endingPoints;
}

StartingPointVector CircuitElement::GetStartPoints() const
{
    StartingPointVector startingPoints;

    for (const auto* startingConnector : m_startingConnectors)
    {
        startingPoints.push_back(startingConnector->GetStartPoint());
    }

    return startingPoints;
}

CircuitElementMimeData CircuitElement::GetMimeData(QPoint eventPos) const
{
    CircuitElementMimeData mimeData(eventPos);
    mimeData.pixmap = m_pixmap;
    mimeData.offset = QPoint(eventPos - pos());
    mimeData.id = GetId();
    mimeData.orderId = m_orderId;
    mimeData.itemSize = this->size();
    mimeData.value = m_outputValue;
    mimeData.itemPosition = pos();
    mimeData.area = QRect(mimeData.itemPosition, mimeData.itemSize);
    mimeData.color = m_color;

    EndingPointVector oldEndingPointVector;
    StartingPointVector oldStartingPointVector;
    for (const auto* endingConnector : m_endingConnectors)
    {
        const auto endPoint = endingConnector->GetEndPoint();
        oldEndingPointVector.push_back(endPoint);
        mimeData.endingPoints.push_back({endPoint.connPos,
                                              endPoint.connId});
    }

    for (const auto* startingConnector : m_startingConnectors)
    {
        const auto startPoint = startingConnector->GetStartPoint();
        oldStartingPointVector.push_back(startPoint);
        mimeData.startingPoints.push_back({startPoint.connPos,
                                                startPoint.connIds});
    }

    for (unsigned int i = 0; i < mimeData.endingPoints.size(); ++i)
    {
        mimeData.oldNewPoints.push_back({oldEndingPointVector[i].connPos,
                                         mimeData.endingPoints[i].connPos});
    }

    for (unsigned int i = 0; i < mimeData.startingPoints.size(); ++i)
    {
        mimeData.oldNewPoints.push_back({oldStartingPointVector[i].connPos,
                                         mimeData.startingPoints[i].connPos});
    }

    return mimeData;
}

bool CircuitElement::IsNumberParameterValid() const
{
    return m_numberParameterIsValid;
}

void CircuitElement::AddActionsToMenu(QMenu* menu)
{
    AddActionSimulateToMenu(menu);
    AddActionChangeColorToMenu(menu);
    AddActionChangeSizeToMenu(menu);
    AddActionDuplicateToMenu(menu);
    BaseCircuitItem::AddActionsToMenu(menu);
}

void CircuitElement::AddActionSimulateToMenu(QMenu* menu)
{
    auto* actionSimulate = new QAction("Simulate", this);
    connect(actionSimulate, &QAction::triggered,
            this, [this] (bool)
            {
                if (m_numberParameterIsValid)
                {
                    emit startFunctionalFaultSimulation(GetId());
                }
                else
                {
                    auto parentWidget = qobject_cast<QWidget*>(parent());
                    if (!parentWidget)
                    {
                        return;
                    }

                    QMessageBox::information(parentWidget,
                            tr("Can't simulate on element #")
                               + QString::number(m_orderId),
                            tr("Element has empty or invalid number parameter.\n"
                               "Please use another one and try again."),
                            QMessageBox::Ok
                            );
                }
            });

    menu->addAction(actionSimulate);
}

void CircuitElement::AddActionChangeSizeToMenu(QMenu* menu)
{
    QAction* actionChangeSize = new QAction("Change Size", parent());
    connect(actionChangeSize, &QAction::triggered,
            this, [this] (bool)
            {
                emit closeDialogs();

                auto parentWidget = qobject_cast<QWidget*>(parent());
                if (!parentWidget)
                {
                    return;
                }

                auto* elementSizeChanger = new ElementSizeChanger(this,
                                                                  parentWidget);
                elementSizeChanger->move(QCursor::pos());
                connect(this, &BaseCircuitItem::closeDialogs,
                        elementSizeChanger, &ElementSizeChanger::close);

                // Magic connect: elementSizeChanger passed as 3rd argument
                // to ensure signal checkAndTryToRebookArea will work only
                // while ElementSizeChanger dialog is opened (and not deleted)
                connect(this, &CircuitElement::checkAndTryToRebookArea,
                        elementSizeChanger, [this](int previousInputsNumber,
                                                   int previousOutputsNumber,
                                                   EndingPointVector oldEndingPointVector,
                                                   StartingPointVector oldStartingPointVector,
                                                   QRect previousArea)
                        {
                            EndingPointVector endPoints = GetEndPoints();
                            StartingPointVector startPoints = GetStartPoints();
                            std::vector<std::pair<QPoint, QPoint>> oldNewPoints;
                            quint64 displacedConnId = 0;
                            StartingPoint::IdsSet displacedConnIdSet;

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
                                    displacedConnId = oldEndingPointVector.back().connId;
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
                                    displacedConnIdSet = oldStartingPointVector.back().connIds;
                                }

                                for (std::size_t i = 0; i < startPoints.size(); ++i)
                                {
                                    oldNewPoints.push_back({oldStartingPointVector[i].connPos,
                                                            startPoints[i].connPos});
                                }
                            }

                            emit tryToRebookArea(this,
                                                 previousArea,
                                                 oldNewPoints,
                                                 displacedConnId,
                                                 displacedConnIdSet,
                                                 previousInputsNumber,
                                                 previousOutputsNumber);
                        });
            });

    menu->addAction(actionChangeSize);
}

void CircuitElement::AddActionDuplicateToMenu(QMenu* menu)
{
    auto* actionDuplicate = new QAction("Duplicate", parent());
    connect(actionDuplicate, &QAction::triggered,
            this, [this] (bool)
            {
                emit closeDialogs();

                auto parentWidget = qobject_cast<QWidget*>(parent());
                if (!parentWidget)
                {
                    return;
                }

                auto* dialogDuplicate =
                    new DialogDuplicateElementItem(this, parentWidget);
                dialogDuplicate->move(QCursor::pos());

                connect(this, &CircuitElement::setOrderIdHintForDuplicate,
                        dialogDuplicate, &DialogDuplicateElementItem::SetOrderId);
                connect(this, &BaseCircuitItem::closeDialogs,
                        dialogDuplicate, &DialogDuplicateElementItem::close);

                emit askOrderIdHint();
            });

    menu->addAction(actionDuplicate);
}
