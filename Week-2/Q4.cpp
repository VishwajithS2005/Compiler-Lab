// A C++ program to write a regex to validate dates in DD-MM-YYYY format.
#include <iostream>
#include <regex>
using namespace std;

int main()
{
    string date;
    cout << "Enter a date in the format \"DD-MM-YYYY\": ";
    cin >> date;

    regex dp("^[0-3][0-9]\\-[0-1][0-9]\\-[0-9]{4}$");

    if (regex_match(date, dp))
    {
        cout << "The given date matches the given format." << endl;
    }
    else
    {
        cout << "The given date doesn't match the given format." << endl;
    }
    return 0;
}