#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

using usize = std::size_t;

string ltrim(const string &);
string rtrim(const string &);
vector<string> split(const string &);

/*
 * Complete the 'plusMinus' function below.
 *
 * The function accepts INTEGER_ARRAY arr as parameter.
 */

void plusMinus(const vector<int> &arr)
{
    auto print = [&arr](std::size_t count) -> void
    {
        std::cout << std::fixed << std::setprecision(6) << (static_cast<double>(count) / static_cast<double>(arr.size())) << '\n';
    };

    std::size_t positiveCount = 0;
    std::size_t negativeCount = 0;
    std::size_t zeroCount = 0;

    for (int value : arr)
    {
        if (value > 0)
        {
            ++positiveCount;
        }
        else if (value < 0)
        {
            ++negativeCount;
        }
        else
        {
            ++zeroCount;
        }
    }

    print(positiveCount);
    print(negativeCount);
    print(zeroCount);
}

int main()
{
    string n_temp;
    getline(cin, n_temp);
    int n = stoi(ltrim(rtrim(n_temp)));
    string arr_temp_temp;
    getline(cin, arr_temp_temp);
    vector<string> arr_temp = split(rtrim(arr_temp_temp));
    vector<int> arr(n);

    for (int i = 0; i < n; i++)
    {
        int arr_item = stoi(arr_temp[i]);
        arr[i] = arr_item;
    }

    plusMinus(arr);
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
