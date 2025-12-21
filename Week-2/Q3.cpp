// A C++ program to write a regex to validate an email address.
#include <iostream>
#include <regex>
using namespace std;

int main()
{
    string emailAddresss;
    cout << "Enter an email address: ";
    cin >> emailAddresss;

    regex ep("^([a-zA-Z]+)([a-zA-Z0-9_.-]*)@([a-zA-Z0-9.-]+)\\.([a-zA-Z]{2,5})$");

    if (regex_match(emailAddresss, ep))
    {
        cout << "This is a valid email address." << endl;
    }
    else
    {
        cout << "This is an invalid email address." << endl;
    }
    return 0;
}