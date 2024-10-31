#ifndef ELEMENTITEM_HPP
#define ELEMENTITEM_HPP

#include "AbstractItem.hpp"

class ElementItem : public AbstractItem
{
    Q_OBJECT
public:
    explicit ElementItem(QObject *parent = nullptr);

    virtual inline ItemType GetItemType() const noexcept override
    { return ItemType::Element; }

    virtual void SetOrderId(int orderId) override;
    virtual int GetOrderId() const override;
    virtual void SetValue(bool value) override;
    virtual bool GetValue() const override;
    virtual void SetNumParam(int numParam) override;
    virtual int GetNumParam() const override;
    virtual void SetInputsSize(std::size_t inputsSize) override;
    virtual std::size_t GetInputsSize() const override;

private:
    int m_orderId = -1;
    int m_numberParam = 0;
    bool m_outputValue = 0;
    std::size_t m_inputsSize;
};

#endif // ELEMENTITEM_HPP
