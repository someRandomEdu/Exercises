#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);

int main()
{
    string n_temp;
    getline(cin, n_temp);
    int n = stoi(ltrim(rtrim(n_temp)));

    if (n <= 9)
    {
        // no thanks i'm too lazy for more if else
        constexpr const char *names[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
        std::cout << names[n - 1];
    }
    else
    {
        std::cout << "Greater than 9";
    }

    return 0;
}

string ltrim(const string &str)
{
    string s(str);

    s.erase(s.begin(), find_if(s.begin(), s.end(), [](char ch) -> bool
                               { return !std::isspace(ch); }));
    return s;
}

string rtrim(const string &str)
{
    string s(str);

    s.erase(find_if(s.rbegin(), s.rend(), [](char ch) -> bool
                    { return !std::isspace(ch); })
                .base(),
            s.end());

    return s;
}
