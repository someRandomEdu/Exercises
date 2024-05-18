#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

int main()
{
    auto printLine = [](const auto &value) noexcept -> void
    {
        std::cout << value << std::endl;
    };

    int integer;
    long longInt;
    char character;
    float f;
    double d;
    std::cin >> integer >> longInt >> character >> f >> d;
    printLine(integer);
    printLine(longInt);
    printLine(character);
    std::cout << std::fixed << std::setprecision(3);
    printLine(f);
    std::cout << std::fixed << std::setprecision(9);
    printLine(d);
    return 0;
}
