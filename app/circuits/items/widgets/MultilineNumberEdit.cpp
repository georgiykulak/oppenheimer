#include "MultilineNumberEdit.hpp"

#include <QTextBlock>

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

    m_textDocument = new QTextDocument(this);
    auto textLayout = new MultilineNumberTextDocumentLayout(m_textDocument, font());
    m_textDocument->setDocumentLayout(textLayout);
    setDocument(m_textDocument);
}

MultilineNumberTextDocumentLayout::MultilineNumberTextDocumentLayout(QTextDocument *textDocument,
                                                                     QFont font)
    : QPlainTextDocumentLayout{textDocument}
    , m_fontMetrics(font)
{

}

QRectF MultilineNumberTextDocumentLayout::blockBoundingRect(const QTextBlock &block) const
{
    QTextLayout* layout = block.layout();
    layout->clearLayout();

    qreal height = 0;
    // For measuring the length of a line based on a fixed number of characters
    // and current font. 4 zeroes stand for binary maximum per line
    QString sampleString = "0000";
    auto calculatedWidth = m_fontMetrics.horizontalAdvance(sampleString);

    layout->beginLayout();
    while (true)
    {
        QTextLine line = layout->createLine();
        if (!line.isValid())
        {
            break;
        }

        line.setLineWidth(calculatedWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
    }
    layout->endLayout();

    return QPlainTextDocumentLayout::blockBoundingRect(block);
}
