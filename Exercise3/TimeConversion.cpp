#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string timeConversion(string timeString) // looks hacky...
{
    int hour = std::stoi(timeString.substr(0, 2));
    bool isPm = timeString.substr(timeString.length() - 2, 2) == "PM";

    if (isPm)
    {
        if (hour < 12)
        {
            hour += 12;
        }
    }
    else if (hour >= 12)
    {
        hour -= 12;
    }

    string hourString = std::to_string(hour);

    if ((!isPm) && hour < 10)
    {
        hourString.insert(hourString.cbegin(), '0');
    }

    return hourString + timeString.substr(2, timeString.length() - 4);
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));
    string s;
    getline(cin, s);
    string result = timeConversion(s);
    fout << result << "\n";
    fout.close();
    return 0;
}
