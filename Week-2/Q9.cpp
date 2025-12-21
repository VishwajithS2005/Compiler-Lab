// A C++ program to check whether the string is accepted or not – the string must contain no consecutive repeating characters.
#include <iostream>
using namespace std;

int main()
{
    string s;
    cout << "Enter a string: ";
    cin >> s;
    bool flag = true;
    for (int i = 0; i < s.size() - 1; i++)
    {
        if (s[i] == s[i + 1])
        {
            flag = false;
            break;
        }
    }
    if (flag)
    {
        cout << "There are no consecutive repeating characters." << endl;
    }
    else
    {
        cout << "There are consecutive repeating characters." << endl;
    }
    return 0;
}