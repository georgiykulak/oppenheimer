#ifndef DRAGANDDROPFRAME_H
#define DRAGANDDROPFRAME_H

#include "config/ProjectConfigurationManager.hpp"
#include "AreaManager.hpp"
#include "IdHandler.hpp"
#include "ItemUtils.hpp"
#include "ItemRegistry.hpp"

#include <QWidget>

class BaseCircuitItem;
class CircuitElement;

class CircuitCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit CircuitCanvas(QWidget *parent = nullptr);
    int GetOrderIdHint(quint64 itemType);

signals:
    // incoming signals
    void setNumberParameterToElementItem(quint64 id, int numParam);

    // outgoing signals
    void addNewItem(quint64 itemType, quint64 id, std::size_t inputsSize);
    void removeItem(quint64 id);
    void changeElementItemInputsSize(quint64 id, std::size_t inputsSize);
    void clearAllItems();
    void setOrderIdHint(quint64 itemType, int orderId);
    void startFunctionalFaultSimulation(quint64 elementId);

public slots:
    void CreateNewCircuit();
    void OpenCircuitFromFile();
    void SaveCircuitToFile();
    void NewSavingFile();
    void TryToRebookArea(CircuitElement* circuitElement,
                         QRect previousArea,
                         std::vector<std::pair<QPoint, QPoint>> oldNewPoints,
                         quint64 displacedConnId,
                         StartingPoint::IdsSet displacedConnIdSet,
                         int previousInputsNumber,
                         int previousOutputsNumber);

protected:
    void paintEvent(QPaintEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    AreaManager m_areaManager;
    IdHandler m_idHandler;
    ProjectConfigurationManager m_projectConfigurator;
    ItemRegistry m_itemRegistry;

    QLine m_currentConnectingLine;

    void ProcessDragEnterEvent(QDragEnterEvent *event);
    void ProcessDragMoveEvent(QDragMoveEvent *event);
    void ProcessDropEvent(QDropEvent *event);
    void ProcessMousePressEvent(QMouseEvent *event);
    void AcceptDndEvent(QDropEvent* baseDndEvent);
    void RemoveConnectionById(quint64 connId);

private slots:
    void RemoveCircuitItem(BaseCircuitItem* item);
    void InsertConnection(quint64 startId,
                          quint64 endId,
                          QLine positions);
    void ClearAll();
};

#endif // DRAGANDDROPFRAME_H
