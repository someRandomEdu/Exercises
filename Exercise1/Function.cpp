#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <initializer_list>

using namespace std;

// This works too, despite it actually being a function object
// auto max_of_four = [](int a, int b, int c, int d) -> int
// {
//     return std::max(std::initializer_list<int>{a, b, c, d});
// };

int max_of_four(int a, int b, int c, int d)
{
    return std::max(std::initializer_list<int>{a, b, c, d});
}

int main()
{
    int a;
    int b;
    int c;
    int d;
    std::cin >> a >> b >> c >> d;
    std::cout << max_of_four(a, b, c, d);
    return 0;
}
