#include "CircuitElement.hpp"
#include "connectors/EndingConnector.hpp"
#include "connectors/StartingConnector.hpp"
#include "widgets/LogicVectorEdit.hpp"
#include "Config.hpp"

#include <QPainter>
#include <QPushButton>

CircuitElement::CircuitElement(const CircuitElementMimeData& mimeData,
                               QWidget *parent,
                               bool numParamEnabled)
    : BaseCircuitItem{parent}
{
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

    SetId(mimeData.id);
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

CircuitElement::~CircuitElement()
{
    m_endingConnectors.clear();
    m_startingConnectors.clear();
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

    emit tryToRebookArea(currentInputsNumber,
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

    emit tryToRebookArea(m_endingConnectors.size(),
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
    for (const auto& endingConnector : m_endingConnectors)
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

void CircuitElement::RemoveConnectionId(quint64 connId)
{
    for (auto* startingConnector : m_startingConnectors)
    {
        startingConnector->RemoveConnectionId(connId);
    }

    for (auto* endingConnector : m_endingConnectors)
    {
        endingConnector->RemoveConnectionId(connId);
    }
}

bool CircuitElement::IsNumberParameterValid() const
{
    return m_numberParameterIsValid;
}

void CircuitElement::SetOrderId(int orderId)
{
    m_orderId = orderId;
    update();
}

void CircuitElement::SetColor(const QColor& color)
{
    m_color = color;
    update();
}

void CircuitElement::paintEvent(QPaintEvent *event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
