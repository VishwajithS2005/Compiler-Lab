// A C++ program to check whether the string is accepted or not – the string must contain exactly two occurrences of 'a'.
#include <iostream>
#include <regex>
using namespace std;

int main()
{
    string s;
    cout << "Enter a string: ";
    cin >> s;

    regex sp("^[a]{2}$");
    if (regex_match(s, sp))
    {
        cout << "The string has exactly 2 occurrences of a." << endl;
    }
    else
    {
        cout << "The string doesn't have exactly 2 occurrences of a." << endl;
    }
    return 0;
}