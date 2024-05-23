#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);

void staircase(int n) 
{
    for (int i = 1; i <= n; ++i)
    {
        int whitespaces = n - i;
        
        for (int j = 0; j < whitespaces; ++j)
        {
            std::cout << ' ';
        }        
        
        for (int j = 0; j < n - whitespaces; ++j)
        {
            std::cout << '#';
        }
        
        std::cout << '\n';
    }
}

int main()
{
    string n_temp;
    getline(cin, n_temp);

    int n = stoi(ltrim(rtrim(n_temp)));

    staircase(n);

    return 0;
}

string ltrim(const string &str)
{
    string s(str);
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](char ch) -> bool { return !std::isspace(ch); }));
    return s;
}

string rtrim(const string &str)
{
    string s(str);
    s.erase(find_if(s.rbegin(), s.rend(), [](char ch) -> bool { return !std::isspace(ch); }).base(), s.end());
    return s;
}
