// A C++ program to check whether the string is accepted or not – the string should be of even length and contain only a and b.
#include <iostream>
#include <regex>
using namespace std;

int main()
{
    string s;
    cout << "Enter a string: ";
    cin >> s;

    regex sp("^[a|b]+$");
    if (regex_match(s, sp) && s.size() % 2 == 0)
    {
        cout << "The string is of even length and contains only a and b." << endl;
    }
    else
    {
        cout << "The string doesn't satisfy one or more of the given conditions." << endl;
    }
    return 0;
}