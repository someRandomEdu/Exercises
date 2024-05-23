#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);

string hackerrankInString(string str)
{
    auto isSubSequence = [](const string &str, const string &sequence) -> bool 
    {
        std::size_t i = 0;
        std::size_t j = 0;

        while (i < str.length() && j < sequence.length())
        {
            if (str[i] == sequence[j])
            {
                ++j;
            }

            ++i;
        }

        return j == sequence.length();
    };

    return isSubSequence(str, "hackerrank") ? "YES" : "NO";
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));
    string q_temp;
    getline(cin, q_temp);
    int q = stoi(ltrim(rtrim(q_temp)));

    for (int q_itr = 0; q_itr < q; q_itr++) 
    {
        string s;
        getline(cin, s);
        string result = hackerrankInString(s);
        fout << result << "\n";
    }

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
