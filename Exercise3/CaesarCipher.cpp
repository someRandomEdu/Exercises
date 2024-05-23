#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

string caesarCipher(string str, int offset)
{
    auto rotate = [](char c, int offset) -> char
    {
        constexpr char alphabetSize = 'z' - 'a' + 1;

        if (c >= 'a' && c <= 'z')
        {
            int temp = static_cast<int>(c) + (offset % static_cast<int>(alphabetSize));

            if (temp > 'z')
            {
                temp -= alphabetSize;
            }
            else if (temp < 'a')
            {
                temp += alphabetSize;
            }

            return static_cast<char>(temp);
        }
        else if (c >= 'A' && c <= 'Z')
        {
            int temp = static_cast<int>(c) + (offset % static_cast<int>(alphabetSize));

            if (temp > 'Z')
            {
                temp -= alphabetSize;
            }
            else if (temp < 'A')
            {
                temp += alphabetSize;
            }

            return static_cast<char>(temp);
        }
        else 
        {
            return c;
        }
    };

    string result = string(str);

    for (char &c : result)
    {
        c = rotate(c, offset);
    }

    return result;
}
