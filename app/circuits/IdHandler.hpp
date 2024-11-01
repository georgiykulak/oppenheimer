#ifndef IDHANDLER_HPP
#define IDHANDLER_HPP

#include <unordered_set>

#include <QObject>

class IdHandler : public QObject
{
    Q_OBJECT
public:
    explicit IdHandler(QObject *parent = nullptr);
    quint64 NewUid();
    void RemoveUid(quint64 id);
    bool NewInputOrderId(int orderId);
    void RemoveInputOrderId(int orderId);
    bool NewOutputOrderId(int orderId);
    void RemoveOutputOrderId(int orderId);
    bool NewElementOrderId(int orderId);
    void RemoveElementOrderId(int orderId);
    quint64 ContainsUid(quint64 uid) const;
    bool ContainsInputOrderId(int orderId) const;
    bool ContainsOutputOrderId(int orderId) const;
    bool ContainsElementOrderId(int orderId) const;
    int GetLastInputOrderId() const;
    int GetLastOutputOrderId() const;
    int GetLastElementOrderId() const;
    void Clear();

private:
    std::unordered_set<quint64> m_uids;
    std::unordered_set<int> m_inputOrderIds;
    std::unordered_set<int> m_outputOrderIds;
    std::unordered_set<int> m_elementOrderIds;
    quint64 m_lastUid = 1;
    int m_lastInputOrderId = 0;
    int m_lastOutputOrderId = 0;
    int m_lastElementOrderId = 0;
};

#endif // IDHANDLER_HPP
