#ifndef BASECIRCUITITEM_HPP
#define BASECIRCUITITEM_HPP

#include "ItemUtils.hpp"

#include <QWidget>

#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class EndingConnector;
class StartingConnector;

struct RequiredItemMeta;

class BaseCircuitItem : public QWidget
{
    Q_OBJECT
public:
    explicit BaseCircuitItem(QWidget* parent = nullptr);
    virtual ~BaseCircuitItem();

    ///////////////////////////////////////////////////////////////////////////

    using JsonProcessor =
            std::function<void (RequiredItemMeta reqMeta,
                                const json& itemMeta,
                                QWidget* canvas)>;
    static std::map<quint64, JsonProcessor>& GetJsonProcessors();
    static void RegisterJsonProcessor(quint64 type,
                                      JsonProcessor processor);
    static void ConstructItemFromJson(RequiredItemMeta reqMeta,
                                      const json& itemMeta,
                                      QWidget* canvas);

    ///////////////////////////////////////////////////////////////////////////



    ///////////////////////////////////////////////////////////////////////////

    virtual ItemType GetItemType() const noexcept
    { WarnNotImplemented(ItemType(Invalid)); }
    quint64 GetId() const;

    virtual void DrawToPixmap() { WarnNotImplemented(); }
    virtual void SetValue(bool) { WarnNotImplemented(); }

    virtual std::vector<EndingConnector*> GetEndingConnectors() const;
    virtual std::vector<StartingConnector*> GetStartingConnectors() const;
    virtual void RemoveConnectionId(quint64 connId);

    virtual void AddActionsToMenu(QMenu* menu);

    virtual json GetJsonMeta() const;

signals:
    bool closeDialogs();
    void removeCircuitItem(BaseCircuitItem* item);

public slots:
    void SetOrderId(int orderId);
    void SetColor(const QColor& color);

protected:
    std::vector<EndingConnector*> m_endingConnectors;
    std::vector<StartingConnector*> m_startingConnectors;
    QPixmap m_pixmap;
    QColor m_color = Qt::white;
    quint64 m_id = 0;
    int m_orderId = -1;

    virtual void paintEvent(QPaintEvent* event) override;

    void AddActionDeleteToMenu(QMenu* menu);
    void AddActionChangeColorToMenu(QMenu* menu);
};

struct RequiredItemMeta
{
    EndingPointVector endingPoints;
    StartingPointVector startingPoints;
    QColor color;
    QSize itemSize;
    QPoint itemPosition;
    quint64 type;
    quint64 id;
    int orderId;
};

#endif // BASECIRCUITITEM_HPP
