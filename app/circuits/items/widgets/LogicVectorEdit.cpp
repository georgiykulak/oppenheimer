#include "LogicVectorEdit.hpp"

#include <QPlainTextEdit>
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>

LogicVectorEdit::LogicVectorEdit(QWidget *parent)
    : QWidget{parent}
{
    m_textEdit = new QPlainTextEdit(this);
    QString styleSheet;
    styleSheet += "background-color: " + QColor(Qt::white).name();
    styleSheet += ";color: " + QColor(Qt::black).name();
    m_textEdit->setStyleSheet(styleSheet);
    m_textEdit->move(2, 2);

    setSizePolicy(m_textEdit->sizePolicy());

    m_textEdit->setFrameStyle(QFrame::NoFrame);
    m_textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_textEdit, &QPlainTextEdit::textChanged,
            this, &LogicVectorEdit::onTextChanged);
}

QSize LogicVectorEdit::sizeHint() const
{
    return m_textEdit->sizeHint() + m_margin;
}

void LogicVectorEdit::setEnabled(bool enable)
{
    m_textEdit->setEnabled(enable);
}

void LogicVectorEdit::setMaximumDigitCount(int digitCount)
{
    m_digitCount = digitCount;
    m_maximumNumber = 1 << digitCount; // 2 ^ N, N = digitCount
}

void LogicVectorEdit::setNotation(bool isBinary)
{
    if (m_isBinaryNotation == isBinary)
        return;

    m_isBinaryNotation = isBinary;
    onTextChanged();
    //m_textEdit->toPlainText()
}

bool LogicVectorEdit::IsNotationBinary() const
{
    return m_isBinaryNotation;
}

void LogicVectorEdit::setNumber(int number)
{
    if (m_isBinaryNotation)
    {
        QString numVector;
        for (int i = 0; i < m_digitCount; ++i)
        {
            numVector += (number & 1) ? "1" : "0";
            number >>= 1;
        }
        std::reverse(numVector.begin(), numVector.end());
        m_textEdit->setPlainText(numVector);
    }
    else
    {
        m_textEdit->setPlainText(QString::number(number));
    }
}

void LogicVectorEdit::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    drawValidityFrame(painter);
}

void LogicVectorEdit::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    m_textEdit->resize(event->size() - m_margin);
}

void LogicVectorEdit::drawValidityFrame(QPainter &painter) const
{
    QPen pen;

    if (m_valid)
    {
        pen.setBrush(Qt::darkGray);
    }
    else
    {
        pen.setBrush(Qt::red);
    }

    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawRoundedRect(0, 0, width(), height(), 17, 17, Qt::RelativeSize);
}

void LogicVectorEdit::onTextChanged()
{
    auto newText = m_textEdit->toPlainText();
    qDebug() << "LogicVectorEdit onTextChanged: new text =" << newText << " maximum number =" << m_maximumNumber;

    bool ok = false;
    int number;
    if (m_isBinaryNotation)
    {
        number = newText.toInt(&ok, 2);
    }
    else
    {
        number = newText.toInt(&ok, 10);
    }

    if (ok && number < m_maximumNumber)
    {
        m_valid = true;
        emit numberChangedAndValid(number);
        emit setNumberValidity(true);
    }
    else
    {
        m_valid = false;
        emit setNumberValidity(false);
    }

    update();
}
