#ifndef OUTPUTITEM_HPP
#define OUTPUTITEM_HPP

#include "AbstractItem.hpp"

class OutputItem : public AbstractItem
{
    Q_OBJECT
public:
    explicit OutputItem(QObject *parent = nullptr);

    virtual inline ItemType GetItemType() const noexcept
    { return ItemType::Output; }

private:
    int m_orderId = -1;
    bool m_outputValue = 0;
};

#endif // OUTPUTITEM_HPP
