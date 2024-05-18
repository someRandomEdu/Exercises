#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

int main()
{
    int length;
    std::cin >> length;
    int *values = new int[length]; // MSVC compiler doesn't support VLAs however...

    for (int i = 0; i < length; ++i)
    {
        std::cin >> values[i];
    }

    for (int i = length - 1; i >= 0; --i)
    {
        std::cout << values[i] << ' ';
    }

    delete[] values;
    return 0;
}
