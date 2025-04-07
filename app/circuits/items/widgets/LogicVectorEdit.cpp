#include "LogicVectorEdit.hpp"

#include <QPlainTextEdit>
#include <QScrollBar>
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

    m_textEdit->setFrameStyle(QFrame::NoFrame);
    m_textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textEdit->document()->setDocumentMargin(1);

    m_textEdit->move(2, 2);

    connect(m_textEdit, &QPlainTextEdit::textChanged,
            this, &LogicVectorEdit::onTextChanged);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setAttribute(Qt::WA_DeleteOnClose);
    show();
}

void LogicVectorEdit::set_sb(QScrollBar* sb)
{
    m_scrollbar = sb;
    m_textEdit->setVerticalScrollBar(m_scrollbar);
}

QSize LogicVectorEdit::sizeHint() const
{
    return m_textEdit->sizeHint() + m_margin;
}

void LogicVectorEdit::setEnabled(bool enable)
{
    m_textEdit->setEnabled(enable);
}

void LogicVectorEdit::setDigitCount(int digitCount)
{
    qDebug() << "LogicVectorEdit setDigitCount: old digit count =" << m_digitCount
             << "new digit count =" << digitCount;

    m_digitCount = digitCount;

    m_logicalVector.resize(m_digitCount);
    m_currentText.resize(m_digitCount);
    for (std::size_t i = 0; i < m_digitCount; ++i)
    {
        m_logicalVector[i] = false;
        m_currentText[i] = '0';
    }

    m_textEdit->setPlainText(m_currentText);

    // 6 - maximum input number of element to fit text in field
    constexpr auto maximumDigitCount = 1 << 6;
    if (m_digitCount > maximumDigitCount)
    {
        m_scrollbar->show();
    }
    else
    {
        m_scrollbar->hide();
    }
}

void LogicVectorEdit::setNotation(bool isBinary)
{
    qDebug() << "LogicVectorEdit setNotation";

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

void LogicVectorEdit::setLogicalVector(const std::vector<bool>& lv)
{
    qDebug() << "LogicVectorEdit setLogicalVector";

    if (m_isBinaryNotation)
    {
        m_logicalVector = lv;

        // Received default state
        if (m_logicalVector.empty())
        {
            m_logicalVector.resize(m_digitCount);
            for (std::size_t i = 0; i < m_digitCount; ++i)
            {
                m_logicalVector[i] = false;
            }
        }

        QString bitVector;
        bitVector.resize(m_logicalVector.size());
        for (std::size_t i = 0; i < m_logicalVector.size(); ++i)
        {
            bitVector[i] = m_logicalVector[i] ? '1' : '0';
        }

        qDebug() << "bitVector:" << bitVector;
        m_textEdit->setPlainText(bitVector);
    }
    else
    {
        // TODO: Convert LV to a string with decimals
        // m_textEdit->setPlainText(...);
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
    painter.drawRoundedRect(0, 0, width(), height(), 3, 3, Qt::AbsoluteSize);
}

void LogicVectorEdit::onTextChanged()
{
    auto newText = m_textEdit->toPlainText();
    if (newText == m_currentText)
    {
        return;
    }
    qDebug() << "LogicVectorEdit onTextChanged: new text =" << newText << "digit count =" << m_digitCount;

    bool valid = true;
    if (m_isBinaryNotation)
    {
        // validate before resizing
        if (newText.size() > m_digitCount)
        {
            auto cursor = m_textEdit->cursor();
            m_textEdit->setPlainText(m_currentText);
            m_textEdit->setCursor(cursor);
            // TODO: Try to use undo instead
            //m_textEdit->undo();
            return;
        }

        if (newText.size() == m_digitCount)
        {
            if (m_logicalVector.size() != m_digitCount)
            {
                m_logicalVector.resize(m_digitCount);
            }

            for (std::size_t i = 0; i < m_digitCount; ++i)
            {
                QChar c = newText[i];

                if (c == '0')
                {
                    m_logicalVector[i] = false;
                }
                else if (c == '1')
                {
                    m_logicalVector[i] = true;
                }
                else
                {
                    valid = false;
                    break;
                }
            }
        }
        {
            valid = false;
        }

        m_currentText = newText;
    }
    else
    {
        // TODO: Convert numeric text to LV
    }

    m_valid = valid;
    if (m_valid)
    {
        emit logicalVectorChangedAndValid(m_logicalVector);
    }
    emit setNumberValidity(m_valid);

    update();
}
