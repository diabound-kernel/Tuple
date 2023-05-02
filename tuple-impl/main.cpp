#include <iostream>

#include "tuple.hpp"

int main()
{
    auto tuple = nonstd::makeTuple(10, " hello world ", 7.4);

    // nonstd::Tuple tuple{10, " hello world ", 7.4};

    nonstd::apply(
        tuple, [](auto &&...args) { (std::cout << ... << args) << std::endl; });
}