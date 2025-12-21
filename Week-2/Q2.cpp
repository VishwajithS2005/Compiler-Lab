// A C++ code to write a regex to validate a 10-digit Indian mobile number starting with digits 6 to 9.
#include <iostream>
#include <regex>
using namespace std;

int main()
{
    string mobileNo;
    cout << "Enter a 10-digit Indian mobile number: ";
    cin >> mobileNo;
    regex pattern("^[6-9][0-9]{9}$");
    if (regex_match(mobileNo, pattern))
    {
        cout << "The mobile number is valid." << endl;
    }
    else
    {
        cout << "The mobile number is invalid." << endl;
    }
}