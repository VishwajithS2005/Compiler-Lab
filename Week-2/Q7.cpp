// A C++ program to check whether the string is accepted or not – the string should begin and end with the same character.
#include <iostream>
using namespace std;

int main()
{
    string s;
    cout << "Enter a string: ";
    cin >> s;
    if (s[0] == s[s.size() - 1])
    {
        cout << "The first and last characters are the same." << endl;
    }
    else
    {
        cout << "The first and last characters are not the same." << endl;
    }
    return 0;
}