#ifndef BASECIRCUITITEM_HPP
#define BASECIRCUITITEM_HPP

#include "ItemUtils.hpp"

#include <vector>

#include <QWidget>

class BaseCircuitItem : public QWidget
{
    Q_OBJECT
public:
    explicit BaseCircuitItem(QWidget *parent = nullptr);

    virtual inline ItemType GetItemType() const noexcept
    { WarnNotImplemented(ItemType(-1)); }

    void SetId(quint64 id);
    quint64 GetId() const;
    void SetSize(QSize size);
    QSize GetSize() const;

    virtual void SetPixmap(const QPixmap&) { WarnNotImplemented();   }
    virtual QPixmap GetPixmap() const      { WarnNotImplemented({}); }
    virtual void DrawToPixmap()            { WarnNotImplemented();   }
    virtual void SetOrderId(int)           { WarnNotImplemented();   }
    virtual int GetOrderId() const         { WarnNotImplemented(-1); }
    virtual void SetValue(bool)            { WarnNotImplemented();   }
    virtual bool GetValue() const          { WarnNotImplemented(0);  }
    virtual void SetNumberParameter(int)   { WarnNotImplemented();   }
    virtual int GetNumberParameter() const { WarnNotImplemented(-1); }

    virtual void RemoveConnectionId(quint64) {};

signals:
    virtual bool closeDialogs();

private:
    quint64 m_id = 0;
    QSize m_size;
};

#endif // BASECIRCUITITEM_HPP
