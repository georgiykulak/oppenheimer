#include "MultilineNumberEdit.hpp"

#include <QTextBlock>
#include <QScrollBar>

MultilineNumberEdit::MultilineNumberEdit(QWidget *parent)
    : QPlainTextEdit{parent}
{
    QString styleSheet;
    styleSheet += "background-color: " + QColor(Qt::white).name();
    styleSheet += ";color: " + QColor(Qt::black).name();
    setStyleSheet(styleSheet);

    setFrameStyle(QFrame::NoFrame);
    setLineWrapMode(QPlainTextEdit::WidgetWidth);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_vScroll = new QScrollBar(this);
    setVerticalScrollBar(m_vScroll);
}
