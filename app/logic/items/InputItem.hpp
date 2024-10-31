#ifndef INPUTITEM_HPP
#define INPUTITEM_HPP

#include "AbstractItem.hpp"

class InputItem : public AbstractItem
{
    Q_OBJECT
public:
    explicit InputItem(QObject *parent = nullptr);

    virtual inline ItemType GetItemType() const noexcept
    { return ItemType::Input; }

private:
    int m_orderId = -1;
    bool m_inputValue = 0;
};

#endif // INPUTITEM_HPP
