#include <iostream>
#include <tuple>

#include "tuple.hpp"

int main()
{
    auto tuple = std::make_tuple(1, 2, 3, 4);

    std::cout << nonstd::apply(tuple, [](auto &&...args) {
        return (args + ...);
    }) << std::endl;
}