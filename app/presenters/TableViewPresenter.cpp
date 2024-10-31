#include "TableViewPresenter.hpp"

#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>

template< typename MatrixType, typename TableItemCreator >
QWidget* createBoolMatrixWidget(const MatrixType& matrix,
                                TableItemCreator createItem)
{
    QTableWidget* tableWidget = new QTableWidget;

    const auto& matrixSize = matrix.size();
    tableWidget->setRowCount(matrixSize);
    tableWidget->setColumnCount(matrixSize);

    for (std::size_t i = 0; i < matrixSize; ++i)
    {
        const auto& vector = matrix[i];
        for (std::size_t j = 0; j < vector.size(); ++j)
        {
            const auto cell = vector[j];
            QTableWidgetItem* newItem = createItem(cell);
            newItem->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(i, j, newItem);
        }
    }

    const auto height = tableWidget->verticalHeader()->sectionSize(0);
    tableWidget->horizontalHeader()->setMaximumSectionSize(height);
    for (typename MatrixType::size_type j = 0; j < matrix.at(0).size(); ++j)
    {
        tableWidget->horizontalHeader()->resizeSection(j, height);
    }

    return tableWidget;
}

TableViewPresenter::TableViewPresenter(QObject* parent)
    : QObject{parent}
{}

void TableViewPresenter::PresentDHQTable(const DHQTable& dhqTable)
{
    QTabWidget* qhdTabs = new QTabWidget;
    qhdTabs->setTabShape(QTabWidget::Rounded);
    qhdTabs->setWindowTitle(
        tr("Simulation for %1 variables").arg(dhqTable.truthTable.size()));
    qhdTabs->setAttribute(Qt::WA_DeleteOnClose);

    QWidget* qTableWidget = createQTableWidget(dhqTable.qMatrix);
    QWidget* hTableWidget = createHTableWidget(dhqTable.hMatrix);
    QWidget* dTableWidget = createDTableWidget(dhqTable.dMatrix);

    qhdTabs->addTab(qTableWidget, "Q-matrix");
    qhdTabs->addTab(hTableWidget, "H-matrix");
    qhdTabs->addTab(dTableWidget, "D-matrix");
    qhdTabs->setCurrentIndex(2);
    qhdTabs->show();
}

QWidget* TableViewPresenter::createHTableWidget(
    const DHQTable::DigitMatrix& hMatrix) const
{
    return createBoolMatrixWidget(
            hMatrix,
            [](std::size_t cell)
            {
                return new QTableWidgetItem(QString::number(cell));
            });
}

QWidget* TableViewPresenter::createQTableWidget(
    const DHQTable::BoolMatrix& qMatrix) const
{
    return createBoolMatrixWidget(
            qMatrix,
            [](bool cell)
            {
                return new QTableWidgetItem(cell ? "1" : "");
            });
}

QWidget* TableViewPresenter::createDTableWidget(
    const DHQTable::BoolMatrix& dMatrix) const
{
    return createBoolMatrixWidget(
            dMatrix,
            [](bool cell)
            {
                return new QTableWidgetItem(cell ? "1" : "");
            });
}
