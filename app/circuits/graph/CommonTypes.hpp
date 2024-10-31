#ifndef COMMONTYPES_HPP
#define COMMONTYPES_HPP

#include <QObject>

#include <limits>
#include <vector>

enum class BookStatus : unsigned
{
    Free = 0,
    Line,
    Item
};

using StatusMatrix = std::vector<std::vector<BookStatus>>;

struct IndexPoint
{
    std::size_t i = std::numeric_limits<std::size_t>::max();
    std::size_t j = std::numeric_limits<std::size_t>::max();

    bool operator<(const IndexPoint& p) const
    {
        if (i < p.i)
        {
            return true;
        }
        else if (p.i < i)
        {
            return false;
        }
        else if (j < p.j)
        {
            return true;
        }

        return false;
    }
};

struct IndexRegion
{
    std::size_t startI = 0;
    std::size_t endI = 0;
    std::size_t startJ = 0;
    std::size_t endJ = 0;
};

using IndexVector = std::vector<IndexPoint>;

#endif // COMMONTYPES_HPP
