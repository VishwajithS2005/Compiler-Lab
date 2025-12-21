// a C++ program to check whether the string is accepted or not – the string must contain both substrings “ab” and “ba” at least once, in any order.
#include <iostream>
#include <regex>
using namespace std;

int main()
{
    string s;
    cout << "Enter a string: ";
    cin >> s;

    regex sp1("ab"), sp2("ba");

    if (regex_search(s, sp1) && regex_search(s, sp2))
    {
        cout << "The string contains both \"ab\" and \"ba\"." << endl;
    }
    else
    {
        cout << "The string doesn't contain either \"ab\" or \"ba\"." << endl;
    }
    return 0;
}