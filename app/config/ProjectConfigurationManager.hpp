#ifndef PROJECTCONFIGURATIONMANAGER_HPP
#define PROJECTCONFIGURATIONMANAGER_HPP

#include "circuits/AreaManager.hpp"
#include "circuits/IdHandler.hpp"

#include <QObject>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class BaseCircuitItem;

class ProjectConfigurationManager : public QObject
{
    Q_OBJECT
public:
    explicit ProjectConfigurationManager(AreaManager& areaManager,
                                         IdHandler& idHandler,
                                         QObject *parent = nullptr);

    void ResetCircuit();

    // To config
    void NewSavingFile();
    void SaveCircuitToFile();

    // From config
    void OpenCircuitFromFile();

signals:
    void addNewInputItem(quint64 id);
    void addNewOutputItem(quint64 id);
    void addNewElementItem(quint64 id, std::size_t inputsSize);
    void insertConnection(quint64 startId,
                          quint64 endId,
                          QLine positions);
    void clearCircuit();

private:
    AreaManager& m_areaManager;
    IdHandler& m_idHandler;
    QString m_fileName;

    // To config
    void SaveCircuitItem(BaseCircuitItem* item, json& metaItems);
    void SaveItemConnections(json& metaConnections);

    // From config
    void ConstructItemsFromJson(const json& metaRoot);
    void ConstructInputItemFromJson(const json& item);
    void ConstructOutputItemFromJson(const json& item);
    void ConstructElementItemFromJson(const json& item);
    void ConstructConnectionFromJson(const json& connection);
};

#endif // PROJECTCONFIGURATIONMANAGER_HPP
