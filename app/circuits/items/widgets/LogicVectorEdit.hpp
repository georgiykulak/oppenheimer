#ifndef LOGICVECTOREDIT_HPP
#define LOGICVECTOREDIT_HPP

#include <QWidget>

class QPlainTextEdit;
class QScrollBar;

class LogicVectorEdit : public QWidget
{
    Q_OBJECT
public:
    explicit LogicVectorEdit(QWidget *parent = nullptr);

    void set_sb(QScrollBar* sb);

    QSize sizeHint() const override;
    void setEnabled(bool enable);
    void setDigitCount(int digitCount);
    void setLogicalVector(const std::vector<bool>& lv);
    void setNotation(bool isBinary);
    bool IsNotationBinary() const;

signals:
    void logicalVectorChangedAndValid(const std::vector<bool>& validLV);
    void setNumberValidity(bool isValid);
    void textRowsCountChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    static constexpr QSize m_margin = QSize(4, 4);

    QPlainTextEdit* m_textEdit;
    QScrollBar* m_scrollbar;
    QString m_currentText = "<invalid>";
    std::vector<bool> m_logicalVector;
    std::size_t m_rows = 1;
    int m_digitCount = 2;
    bool m_valid = true;
    bool m_isBinaryNotation = true;

    void drawValidityFrame(QPainter& painter) const;

private slots:
    void onTextChanged();
};

#endif // LOGICVECTOREDIT_HPP
