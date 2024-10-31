#ifndef PRESENTERTYPES_HPP
#define PRESENTERTYPES_HPP

#include <vector>

struct DHQTable
{
    using BoolMatrix = std::vector<std::vector<bool>>;
    using DigitMatrix = std::vector<std::vector<std::size_t>>;

    DigitMatrix hMatrix;
    BoolMatrix qMatrix;
    BoolMatrix dMatrix;
    BoolMatrix truthTable;
    std::vector<bool> qVector;
};

#endif // PRESENTERTYPES_HPP
