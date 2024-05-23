#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);
vector<string> split(const string &);

string kangaroo(int x1, int v1, int x2, int v2) 
{
    auto actualKangaroo = [=]() -> bool
    {
        if (x1 == x2)
        {
            return true;
        }
        else if (v1 == v2)
        {
            return false;
        }
        else if ((x1 < x2) == (v1 < v2)) 
        {
            return false;
        }
        else 
        {
            return std::abs(x1 - x2) % std::abs(v1 - v2) == 0;
        }
    };
    
    return actualKangaroo() ? string("YES") : string("NO");
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));
    string first_multiple_input_temp;
    getline(cin, first_multiple_input_temp);
    vector<string> first_multiple_input = split(rtrim(first_multiple_input_temp));
    int x1 = stoi(first_multiple_input[0]);
    int v1 = stoi(first_multiple_input[1]);
    int x2 = stoi(first_multiple_input[2]);
    int v2 = stoi(first_multiple_input[3]);
    string result = kangaroo(x1, v1, x2, v2);
    fout << result << "\n";
    fout.close();
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

vector<string> split(const string &str)
{
    vector<string> tokens;
    string::size_type start = 0;
    string::size_type end = 0;

    while ((end = str.find(" ", start)) != string::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }

    tokens.push_back(str.substr(start));
    return tokens;
}