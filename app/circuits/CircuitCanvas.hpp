#ifndef DRAGANDDROPFRAME_H
#define DRAGANDDROPFRAME_H

#include "AreaManager.hpp"
#include "IdHandler.hpp"

#include <QWidget>
#include <QFile>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class BaseCircuitItem;

class CircuitCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit CircuitCanvas(QWidget *parent = nullptr);
    int GetInputOrderIdHint();
    int GetOutputOrderIdHint();
    int GetElementOrderIdHint();

signals:
    // incoming signals
    void setNumberParameterToElementItem(quint64 id, int numParam);

    // outgoing signals
    void addNewInputItem(quint64 id);
    void addNewOutputItem(quint64 id);
    void addNewElementItem(quint64 id, std::size_t inputsSize);
    void removeItem(quint64 id);
    void changeElementItemInputsSize(quint64 id, std::size_t inputsSize);
    void clearAllItems();
    void setInputOrderIdHint(int orderId);
    void setOutputOrderIdHint(int orderId);
    void setElementOrderIdHint(int orderId);
    void startFunctionalFaultSimulation(quint64 elementId);

public slots:
    void CreateNewCircuit();
    void OpenCircuitFromFile();
    void SaveCircuitToFile();
    void NewSavingFile();

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
    QString m_fileName;

    QLine m_currentConnectingLine;

    void ProcessDragEnterEvent(QDragEnterEvent *event);
    void ProcessDragMoveEvent(QDragMoveEvent *event);
    void ProcessDropEvent(QDropEvent *event);
    void ProcessMousePressEvent(QMouseEvent *event);
    void AcceptDndEvent(QDropEvent* baseDndEvent);
    void ClearAll();
    void RemoveCircuitItem(BaseCircuitItem* item);
    void RemoveConnectionById(quint64 connId);
    void InsertConnection(quint64 startId,
                          quint64 endId,
                          QLine positions);
    void SaveCircuitItem(BaseCircuitItem* item, json& metaItems);
    void SaveItemConnections(json& metaConnections);
    void ConstructItemsFromJson(const json& metaRoot);
    void ConstructInputItemFromJson(const json& item);
    void ConstructOutputItemFromJson(const json& item);
    void ConstructElementItemFromJson(const json& item);
    void ConstructConnectionFromJson(const json& connection);
};

#endif // DRAGANDDROPFRAME_H
