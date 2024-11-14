#include "CircuitCanvas.hpp"
#include "items/BaseCircuitItem.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>

CircuitCanvas::CircuitCanvas(QWidget *parent)
    : QWidget{parent}
    , m_areaManager{this}
{
    setAcceptDrops(true);
    m_areaManager.SetMatrixSize(QSize(1280, 720));
}

int CircuitCanvas::GetInputOrderIdHint()
{
    return m_idHandler.GetLastInputOrderId();
}

int CircuitCanvas::GetOutputOrderIdHint()
{
    return m_idHandler.GetLastOutputOrderId();
}

int CircuitCanvas::GetElementOrderIdHint()
{
    return m_idHandler.GetLastElementOrderId();
}

void CircuitCanvas::CreateNewCircuit()
{
    QObjectList childList = this->children();
    qDebug() << "CreateNewCircuit: childs:" << childList.size();
    // childList.size() == 3 means no items placed on canvas
    if (childList.size() > 3)
    {
        auto button = QMessageBox::question(this, tr("New Circuit"),
                        tr("Do you want to save current circuit?"),
                        QMessageBox::Yes,
                        QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            SaveCircuitToFile();
        }
    }
    // Forget filename for saving
    m_fileName = "";

    ClearAll();
}

void CircuitCanvas::OpenCircuitFromFile()
{
    auto fileName = QFileDialog::getOpenFileName(this, "Open Circuit");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    CreateNewCircuit();

    m_fileName = fileName;
}

void CircuitCanvas::SaveCircuitToFile()
{
    qDebug() << "Saving circuit to file:" << m_fileName;

    if (m_fileName.isEmpty())
    {
        m_fileName = QFileDialog::getSaveFileName(this, "Save Circuit As");
    }

    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QObjectList childList = this->children();
    qDebug() << "Saving circuit, items:" << childList.size();

    json metaRoot;
    auto& metaItems = metaRoot["items"];
    auto& metaConnections = metaRoot["connections"];

    for (auto* obj : childList)
    {
        auto* item = qobject_cast<BaseCircuitItem*>(obj);
        if (item)
        {
            SaveCircuitItem(item, metaItems);
        }
    }

    for (auto&& [_, connPair] : m_areaManager.GetConnectedItemIds())
    {
        auto&& [startId, endId] = connPair;
        metaConnections.push_back({startId, endId});
    }

    QTextStream out(&file);
    out << metaRoot.dump(4).c_str();
}

void CircuitCanvas::NewSavingFile()
{
    qDebug() << "NewSavingFile: Filename to reset:" << m_fileName;
    m_fileName = "";

    SaveCircuitToFile();
}

void CircuitCanvas::paintEvent(QPaintEvent *event)
{
    (void)event;

    QPainter painter(this);
    QPen mPen;

#ifdef VISUAL_DEBUGGING
    // For debug purposes
    mPen.setColor(Qt::lightGray);
    mPen.setWidth(1);
    painter.setPen(mPen);
    for (const auto& line : m_areaManager.GetNetLines())
        painter.drawLine(line);

    mPen.setColor(Qt::red);
    mPen.setWidth(1);
    painter.setPen(mPen);
    painter.setBrush(Qt::red);
    for (const auto& point : m_areaManager.GetBookedPoints())
    {
        if (point.second)
            painter.drawEllipse(point.first, 2, 2);
    }

    mPen.setColor(Qt::blue);
    mPen.setWidth(1);
    painter.setPen(mPen);
    painter.setBrush(Qt::blue);
    for (const auto& point : m_areaManager.GetBookedPoints())
    {
        if (!point.second)
            painter.drawEllipse(point.first, 2, 2);
    }
#endif // VISUAL_DEBUGGING

    mPen.setColor(Qt::black);
    mPen.setWidth(2);
    painter.setPen(mPen);
    painter.drawLine(m_currentConnectingLine);
    for (const auto& line : m_areaManager.GetConnections())
    {
        //qDebug() << "Drawing line -> { start =" << line.front()
        //         << "end =" << line.back() << '}';
        painter.drawPolyline(line);
    }
}

void CircuitCanvas::dragEnterEvent(QDragEnterEvent *event)
{
    ProcessDragEnterEvent(event);
}

void CircuitCanvas::dragMoveEvent(QDragMoveEvent *event)
{
    ProcessDragMoveEvent(event);
}

void CircuitCanvas::dragLeaveEvent(QDragLeaveEvent*)
{
    m_currentConnectingLine = {};
    update();
}

void CircuitCanvas::dropEvent(QDropEvent *event)
{
    ProcessDropEvent(event);
}

void CircuitCanvas::mousePressEvent(QMouseEvent *event)
{
    ProcessMousePressEvent(event);
}
