#include "ItemRegistry.hpp"

ItemRegistry::ItemRegistry(QObject *parent)
    : QObject{parent}
{}

const ItemRegistry::MimeProcessorRegistry &ItemRegistry::GetMimeProcessors() const
{
    return m_mimeProcessors;
}

void ItemRegistry::RegisterMimeProcessor(const QString& mimeType,
                                         quint64 itemType,
                                         MimeProcessor processor)
{
    if (mimeType.isEmpty() || itemType == 0)
    {
        qCritical() << "RegisterMimeProcessor: Can't register invalid item type,"
                    << "mimeType is" << mimeType << "itemType is" << itemType;
        return;
    }

    qDebug() << "Registering mime processor for mime type" << mimeType;

    if (m_mimeProcessors.contains(mimeType))
    {
        qWarning() << "RegisterItemType: overriding callback for mime type"
                   << mimeType;
    }

    m_mimeProcessors[mimeType] = {itemType, processor};
}

void ItemRegistry::ConstructItemFromStream(const QString& mimeType,
                                           const BaseCircuitItemMimeData& mimeData,
                                           QDataStream& additionalData)
{
    if (!m_mimeProcessors.contains(mimeType))
    {
        throw std::runtime_error("Mime processor is not registered for mime "
                                 + mimeType.toStdString());
    }

    auto& [_, processor] = m_mimeProcessors.at(mimeType);
    processor(mimeData, additionalData, this);
}
