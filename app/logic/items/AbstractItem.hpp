#ifndef ABSTRACTITEM_H
#define ABSTRACTITEM_H

#include "ItemUtils.hpp"

#include <QObject>

class AbstractItem : public QObject
{
public:
    explicit AbstractItem(QObject *parent = nullptr)
        : QObject{parent}
    {}

    virtual inline ItemType GetItemType() const noexcept = 0;

    virtual void SetOrderId(int)              { WarnNotImplemented();   }
    virtual int GetOrderId() const            { WarnNotImplemented(-1); }
    virtual void SetValue(bool)               { WarnNotImplemented();   }
    virtual bool GetValue() const             { WarnNotImplemented(0);  }
    virtual void SetLogicalVector(const std::vector<bool>&)
    { WarnNotImplemented(); }
    virtual const std::vector<bool>& GetLogicalVector() const
    {
        static std::vector<bool> vec;
        WarnNotImplemented(vec);
    }
    virtual void SetInputsSize(std::size_t)   { WarnNotImplemented();   }
    virtual std::size_t GetInputsSize() const { WarnNotImplemented(0);  }
};

#endif // ABSTRACTITEM_H
