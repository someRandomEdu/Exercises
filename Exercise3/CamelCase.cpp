#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int camelcase(string str)
{
    auto isLowercase = [](char c) -> bool 
    {
        return c >= 'a' && c <= 'z';
    };

    int result = 1;

    for (std::size_t i = 0; i + 1 < str.length(); ++i)
    {
        if (isLowercase(str[i]) != isLowercase(str[i + 1]))
        {
            ++result;
            ++i;
        }
    }

    return result;
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));
    string s;
    getline(cin, s);
    int result = camelcase(s);
    fout << result << "\n";
    fout.close();
    return 0;
}
