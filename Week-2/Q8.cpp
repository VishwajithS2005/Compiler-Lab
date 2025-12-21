// A C++ program to check whether the string is accepted or not – the string must end with “abb”.
#include <iostream>
using namespace std;

int main()
{
    string s;
    cout << "Enter a string: ";
    cin >> s;
    if (s.size() >= 3 && s.substr(s.size() - 3) == "abb")
    {
        cout << "The string ends with \"abb\"." << endl;
    }
    else
    {
        cout << "The string does not end with \"abb\"." << endl;
    }
    return 0;
}
