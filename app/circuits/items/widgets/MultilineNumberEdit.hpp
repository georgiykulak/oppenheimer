#ifndef MULTILINENUMBEREDIT_H
#define MULTILINENUMBEREDIT_H

#include <QPlainTextEdit>

class QScrollBar;

class MultilineNumberEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MultilineNumberEdit(QWidget *parent = nullptr);

    static constexpr std::size_t kBinaryPerRowMaximum = 8;

private:
    QScrollBar* m_vScroll;
};

#endif // MULTILINENUMBEREDIT_H
