#ifndef DRAGANDDROPFRAME_H
#define DRAGANDDROPFRAME_H

#include "AreaManager.hpp"
#include "IdHandler.hpp"

#include <QWidget>

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
    void changeElementItemInputsSize(quint64 id, std::size_t inputsSize);
    void setInputOrderIdHint(int orderId);
    void setOutputOrderIdHint(int orderId);
    void setElementOrderIdHint(int orderId);
    void setEndingInitiatorConnectionId(quint64 connId);
    void insertStartingInitiatorConnectionId(quint64 connId);
    void startFunctionalFaultSimulation(quint64 elementId);

public slots:
    void CreateNewCircuit();

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

    QLine m_currentConnectingLine;

    void ProcessDragEnterEvent(QDragEnterEvent *event);
    void ProcessDragMoveEvent(QDragMoveEvent *event);
    void ProcessDropEvent(QDropEvent *event);
    void ProcessMousePressEvent(QMouseEvent *event);
    void AcceptDndEvent(QDropEvent* baseDndEvent);
    void RemoveConnectionById(quint64 connId);
};

#endif // DRAGANDDROPFRAME_H
