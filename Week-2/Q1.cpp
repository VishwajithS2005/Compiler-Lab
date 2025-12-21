// A C++ program to write a regex to check if a string contains only alphabets.
#include <iostream>
#include <regex>
using namespace std;

int main()
{
    string str;
    cout << "Enter a string: ";
    cin >> str;

    regex pattern("^[A-Za-z]+$");

    if (regex_match(str, pattern))
    {
        cout << "The string contains only alphabets." << endl;
    }
    else
    {
        cout << "The string contains characters other than alphabets." << endl;
    }

    return 0;
}