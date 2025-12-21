// A C++ program to check whether the string is accepted or not – the string length must be a prime number, and the string must contain only 0s and 1s.
#include <iostream>
#include <regex>
#include <math.h>
using namespace std;

bool isPrime(int n)
{
    if (n <= 1)
    {
        return false;
    }
    for (int i = 2; i <= sqrt(n); i++)
    {
        if (n % i == 0)
        {
            return false;
        }
    }
    return true;
}

int main()
{
    string s;
    cout << "Enter a string: ";
    cin >> s;

    regex sp("^[0|1]+$");
    if (regex_match(s, sp) && isPrime(s.size()))
    {
        cout << "The string is of prime number length and contains only 0 and 1." << endl;
    }
    else
    {
        cout << "The string doesn't satisfy one or more of the given conditions." << endl;
    }
    return 0;
}