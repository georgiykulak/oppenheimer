#include "CircuitElement.hpp"
#include "connectors/EndingConnector.hpp"
#include "connectors/StartingConnector.hpp"
#include "widgets/LogicVectorEdit.hpp"

#include <QPainter>
#include <QPushButton>

CircuitElement::CircuitElement(const EndingPointVector& endPoints,
                               const StartingPointVector& startPoints,
                               QWidget *parent,
                               QSize itemSize,
                               bool numParamEnabled)
    : BaseCircuitItem{parent}
{
    m_offsetBetweenConnection = 30;
    m_minimumHeight = 110;
    m_minimumYShift = 55;
    QSize size;
    if (itemSize.isValid())
    {
        size = itemSize;
    }
    else
    {
        const auto realHeight = m_minimumHeight + m_offsetBetweenConnection *
                           (std::max(endPoints.size(), startPoints.size()) - 1);
        size = QSize(110, realHeight);
    }
    setMinimumSize(size);
    setMaximumSize(size);
    SetSize(QSize(size));
    m_pixmap = QPixmap(GetSize());
    m_pixmap.fill(QColor(Qt::transparent));

    QPixmap pixmap(QSize(12, 12)); // Connector size
    pixmap.fill(QColor(Qt::transparent));
    QPainter painter(&pixmap);

    //////////////////////////////////////////////////////////////////////////////////////////

    int i = 0;
    for (const auto& endPoint : endPoints)
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
    for (const auto& startPoint : startPoints)
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

    const auto vectorSize = 1 << endPoints.size(); // 2 ^ N
    m_textField->setMaximumDigitCount(vectorSize);
    m_textField->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_textField, &LogicVectorEdit::numberChangedAndValid,
            this, [this](int number)
            {
                setNumberParameter(number);
                emit setNumberParameterToElementItem(
                    GetId(), GetNumberParameter()
                );
            });
    connect(m_textField, &LogicVectorEdit::setNumberValidity,
            this, [this](bool isValid)
            {
                m_numberParameterIsValid = isValid;
            });
    connect(m_textField, &LogicVectorEdit::textRowsCountChanged,
            this, [this](QString previousNumberParameter)
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
}

CircuitElement::~CircuitElement()
{
    m_endingConnectors.clear();
    m_startingConnectors.clear();
}

void CircuitElement::SetPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
}

QPixmap CircuitElement::GetPixmap() const
{
    return m_pixmap;
}

void CircuitElement::DrawToPixmap()
{
    QPainter painter(&m_pixmap);

    QPen mPen;
    mPen.setColor(m_color);
    mPen.setWidth(2);
    painter.setPen(mPen);
    painter.setBrush(m_color);
    int wBig = 90;
    int hBig = GetSize().height();
    qreal xRad = 18 - MaxConnectors();
    qreal yRad = 18 - MaxConnectors();
    painter.drawRoundedRect(10, 0, wBig, hBig, xRad, yRad, Qt::RelativeSize);
    int xStartText = 25;
    int yStartText = 45;
    int wSmall = 60;
    int hSmall = m_textField->height();
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

void CircuitElement::SetOrderId(int orderId)
{
    m_orderId = orderId;
}

int CircuitElement::GetOrderId() const
{
    return m_orderId;
}

void CircuitElement::SetNumberParameter(int numberParam)
{
    m_numberParam = numberParam;
    m_textField->setNumber(numberParam);
}

int CircuitElement::GetNumberParameter() const
{
    return m_numberParam;
}

void CircuitElement::SetValue(bool value)
{
    m_outputValue = value;
}

bool CircuitElement::GetValue() const
{
    return m_outputValue;
}

void CircuitElement::SetColor(const QColor &color)
{
    m_color = color;
}

QColor CircuitElement::GetColor() const
{
    return m_color;
}

void CircuitElement::SetNotation(bool isBinary)
{
    m_textField->setNotation(isBinary);
    if (isBinary)
    {
        m_notationSwitchButton->setText("bin");
    }
    else
    {
        m_notationSwitchButton->setText("dec");
    }
}

bool CircuitElement::IsNotationBinary() const
{
    return m_textField->IsNotationBinary();
}

void CircuitElement::setNumberParameter(int param)
{
    m_numberParam = param;
}

void CircuitElement::paintEvent(QPaintEvent *event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}

void CircuitElement::SetInputsNumber(int size)
{
    if (size <= 0)
        return;

    const std::size_t number = size;

    const auto vectorSize = 1 << number; // 2 ^ N
    m_textField->setMaximumDigitCount(vectorSize);
    m_textField->setNumber(m_numberParam);

    if (number > m_endingConnectors.size())
    {
        const int newHeight = m_minimumHeight + (number - 1) * m_offsetBetweenConnection;
        if (newHeight > height())
        {
            const auto calculatedHeight =
                25 + m_textField->height() + m_notationSwitchButton->height() + 25;

            QSize newWidgetSize;
            if (calculatedHeight > newHeight)
            {
                newWidgetSize = {width(), calculatedHeight};
            }
            else
            {
                newWidgetSize = {width(), newHeight};
            }

            setMinimumSize(newWidgetSize);
            setMaximumSize(newWidgetSize);
            SetSize(newWidgetSize);

            m_pixmap = QPixmap(GetSize());
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

        update();
    }
    else if (number < m_endingConnectors.size())
    {
        for (auto i = m_endingConnectors.size() - 1; i != number - 1; --i)
        {
            m_endingConnectors[i]->close();
        }

        m_endingConnectors.resize(number);

        const int newHeight = m_minimumHeight + (number - 1) * m_offsetBetweenConnection;
        if (newHeight < height() && number >= m_startingConnectors.size())
        {
            QSize newWidgetSize(width(), newHeight);
            setMinimumSize(newWidgetSize);
            setMaximumSize(newWidgetSize);
            SetSize(newWidgetSize);
        }

        m_pixmap = QPixmap(GetSize());
        m_pixmap.fill(QColor(Qt::transparent));
        DrawToPixmap();
    }
}

void CircuitElement::SetInputsNumberAndRebook(int size)
{
    const int currentInputsNumber = static_cast<int>(GetEndPoints().size());
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

    QRect currentArea(pos(), GetSize());

    // Try upcoming changes
    SetInputsNumber(size);

    emit tryToRebookArea(currentInputsNumber,
                         GetStartPoints().size(),
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
            setMinimumSize(newWidgetSize);
            setMaximumSize(newWidgetSize);
            SetSize(newWidgetSize);

            m_pixmap = QPixmap(GetSize());
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

        update();
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
            setMinimumSize(newWidgetSize);
            setMaximumSize(newWidgetSize);
            SetSize(newWidgetSize);
        }

        m_pixmap = QPixmap(GetSize());
        m_pixmap.fill(QColor(Qt::transparent));
        DrawToPixmap();
    }
}

void CircuitElement::SetOutputsNumberAndRebook(int size)
{
    const int currentOutputsNumber = static_cast<int>(GetStartPoints().size());
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

    QRect currentArea(pos(), GetSize());

    // Try upcoming changes
    SetOutputsNumber(size);

    emit tryToRebookArea(GetEndPoints().size(),
                         currentOutputsNumber,
                         currentEndingPointVector,
                         currentStartingPointVector,
                         currentArea);
}

EndingPointVector CircuitElement::GetEndPoints() const
{
    EndingPointVector endingPointVector;

    for (const auto* endingConnector : m_endingConnectors)
    {
        endingPointVector.push_back(endingConnector->GetEndPoint());
    }

    return endingPointVector;
}

StartingPointVector CircuitElement::GetStartPoints() const
{
    StartingPointVector startingPointVector;

    for (const auto* startingConnector : m_startingConnectors)
    {
        startingPointVector.push_back(startingConnector->GetStartPoint());
    }

    return startingPointVector;
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

int CircuitElement::GetOffsetBetweenConnectionPoints() const
{
    return m_offsetBetweenConnection;
}

bool CircuitElement::IsNumberParameterValid() const
{
    return m_numberParameterIsValid;
}

void CircuitElement::SetPreviousNumberParameter(int height)
{
    m_textField->SetPlainText(m_previousNumberParameter);
}

std::size_t CircuitElement::MaxConnectors() const
{
    return std::max(m_startingConnectors.size(), m_endingConnectors.size());
}
