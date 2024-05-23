#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);
vector<string> split(const string &);

int divisibleSumPairs(int n, int k, vector<int> values) 
{    
    std::vector<std::pair<int, int>> indexPairs = std::vector<std::pair<int, int>>();
    
    auto notContains = [&indexPairs](const std::pair<int, int> &value) -> bool
    {
        for (const std::pair<int, int> &pair : indexPairs)
        {
            if (pair == value)
            {
                return false;
            }
        }
        
        return true;            
    };
    
    
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        for (std::size_t j = i + 1; j < values.size(); ++j)
        {
            if (((values[i] + values[j]) % k == 0) && notContains(std::pair<int, int>(static_cast<int>(i), static_cast<int>(j))))
            {
                indexPairs.push_back(std::pair<int, int>(static_cast<int>(i), static_cast<int>(j)));
            }
        }
    }
    
    return static_cast<int>(indexPairs.size());
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));
    string first_multiple_input_temp;
    getline(cin, first_multiple_input_temp);
    vector<string> first_multiple_input = split(rtrim(first_multiple_input_temp));
    int n = stoi(first_multiple_input[0]);
    int k = stoi(first_multiple_input[1]);
    string ar_temp_temp;
    getline(cin, ar_temp_temp);
    vector<string> ar_temp = split(rtrim(ar_temp_temp));
    vector<int> ar(n);

    for (int i = 0; i < n; i++)
    {
        int ar_item = stoi(ar_temp[i]);
        ar[i] = ar_item;
    }

    int result = divisibleSumPairs(n, k, ar);
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
