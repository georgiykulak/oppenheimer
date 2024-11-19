#ifndef ITEMREGISTRY_HPP
#define ITEMREGISTRY_HPP

#include "items/mime/BaseCircuitItemMimeData.hpp"

#include <QObject>

class BaseCircuitItem;
class CircuitElement;

class ItemRegistry : public QObject
{
    Q_OBJECT
public:
    explicit ItemRegistry(QObject *parent = nullptr);

    using MimeProcessor =
        std::function<void (const BaseCircuitItemMimeData& mimeData,
                            QDataStream& additionalData,
                            ItemRegistry* itemRegistry)>;
    using MimeProcessorRegistry =
        std::map<QString, std::pair<quint64, MimeProcessor>>;

    const MimeProcessorRegistry& GetMimeProcessors() const;
    void RegisterMimeProcessor(const QString& mimeType,
                               quint64 itemType,
                               MimeProcessor processor);
    void ConstructItemFromStream(const QString& mimeType,
                                 const BaseCircuitItemMimeData& mimeData,
                                 QDataStream& additionalData);

signals:
    void removeCircuitItem(BaseCircuitItem* item);
    void setNumberParameterToElementItem(quint64 id, int numParam);
    void startFunctionalFaultSimulation(quint64 elementId);
    void askOrderIdHint();
    void setOrderIdHintForDuplicate(quint64 itemType, int orderId);
    void tryToRebookArea(CircuitElement* circuitElement,
                         QRect previousArea,
                         std::vector<std::pair<QPoint, QPoint>> oldNewPoints,
                         quint64 displacedConnId,
                         StartingPoint::IdsSet displacedConnIdSet,
                         int previousInputsNumber,
                         int previousOutputsNumber);

private:
    MimeProcessorRegistry m_mimeProcessors;
};

#endif // ITEMREGISTRY_HPP
