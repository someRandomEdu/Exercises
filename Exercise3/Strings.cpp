#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

using std::string;

int main() 
{
    string lhs;
    string rhs;
    std::cin >> lhs >> rhs;
    std::cout << lhs.size() << ' ' << rhs.size() << '\n';
    std::cout << (lhs + rhs) << '\n';
    std::swap(lhs.front(), rhs.front());
    std::cout << lhs << ' ' << rhs;
    return 0;
}
