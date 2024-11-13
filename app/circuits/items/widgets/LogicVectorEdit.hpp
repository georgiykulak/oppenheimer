#ifndef LOGICVECTOREDIT_HPP
#define LOGICVECTOREDIT_HPP

#include <QWidget>

class MultilineNumberEdit;

class LogicVectorEdit : public QWidget
{
    Q_OBJECT
public:
    explicit LogicVectorEdit(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    void setEnabled(bool enable);
    void setMaximumDigitCount(int digitCount);
    void setNumber(int number);
    void setNotation(bool isBinary);
    bool IsNotationBinary() const;
    void SetPlainText(QString previousText);

signals:
    void numberChangedAndValid(int validNumber);
    void setNumberValidity(bool isValid);
    void textRowsCountChanged(QString previousText);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    static constexpr QSize m_margin = QSize(4, 4);

    MultilineNumberEdit* m_textEdit;
    QString m_currentText;
    std::size_t m_rows = 1;
    int m_digitCount = 1;
    int m_maximumNumber = 1;
    bool m_valid = true;
    bool m_isBinaryNotation = true;

    void drawValidityFrame(QPainter& painter) const;

private slots:
    void onTextChanged();
};

#endif // LOGICVECTOREDIT_HPP
