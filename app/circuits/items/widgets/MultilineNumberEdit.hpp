#ifndef MULTILINENUMBEREDIT_H
#define MULTILINENUMBEREDIT_H

#include <QPlainTextEdit>

class QTextDocument;

class MultilineNumberEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MultilineNumberEdit(QWidget *parent = nullptr);

    static constexpr std::size_t kBinaryPerRowMaximum = 8;

private:
    QTextDocument* m_textDocument;
};

class MultilineNumberTextDocumentLayout : public QPlainTextDocumentLayout
{
    Q_OBJECT
public:
    explicit MultilineNumberTextDocumentLayout(QTextDocument* textDocument,
                                               QFont font);

    QRectF blockBoundingRect(const QTextBlock &block) const override;

private:
    QFontMetrics m_fontMetrics;
    int m_calculatedWidth;
};

#endif // MULTILINENUMBEREDIT_H
