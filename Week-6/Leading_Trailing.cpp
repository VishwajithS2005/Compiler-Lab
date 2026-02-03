#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <limits>
using namespace std;

struct NonTerminal
{
    char nt;
    vector<string> rhs;
    set<char> lead;
    set<char> trail;
};

vector<struct NonTerminal> p;

bool isNonTerminal(char c)
{
    for (struct NonTerminal temp_p : p)
    {
        if (temp_p.nt == c)
        {
            return true;
        }
    }
    return false;
}

set<char> getLeading(char nt)
{
    set<char> lead;
    for (struct NonTerminal temp_p : p)
    {
        if (temp_p.nt == nt)
        {
            for (string s : temp_p.rhs)
            {
                if (!isNonTerminal(s[0]))
                {
                    lead.insert(s[0]);
                }
                else if (s.length() > 1)
                {
                    if (!isNonTerminal(s[1]))
                    {
                        lead.insert(s[1]);
                    }
                    if (s[0] != temp_p.nt)
                    {
                        for (char c : getLeading(s[0]))
                        {
                            lead.insert(c);
                        }
                    }
                }
            }
        }
    }
    return lead;
}

set<char> getTrailing(char nt)
{
    set<char> trail;
    for (struct NonTerminal temp_p : p)
    {
        if (temp_p.nt == nt)
        {
            for (string s : temp_p.rhs)
            {
                if (!isNonTerminal(s[s.length() - 1]))
                {
                    trail.insert(s[s.length() - 1]);
                }
                else if (s.length() > 1)
                {
                    if (!isNonTerminal(s[s.length() - 2]))
                    {
                        trail.insert(s[s.length() - 2]);
                    }
                    if (s[s.length() - 1] != temp_p.nt)
                    {
                        for (char c : getTrailing(s[s.length() - 1]))
                        {
                            trail.insert(c);
                        }
                    }
                }
            }
        }
    }
    return trail;
}

void createStruct(string s)
{
    struct NonTerminal temp;
    vector<string> temp_rhs;
    int i = 0;
    bool start_rhs = false;
    string temp_r = "", temp_l = "";
    while (i < s.length())
    {
        if (s[i] == ' ')
        {
            i++;
            continue;
        }
        if (start_rhs)
        {
            if (s[i] != '|')
            {
                temp_r += s[i];
            }
            else
            {
                temp_rhs.push_back(temp_r);
                temp_r = "";
            }
        }
        else
        {
            if (s[i] == '-' && s[i + 1] == '>')
            {
                start_rhs = true;
                i += 2;
            }
            else
                temp_l += s[i];
        }
        i++;
    }
    temp_rhs.push_back(temp_r);
    temp.nt = temp_l[0];
    temp.rhs = temp_rhs;
    p.push_back(temp);
}

int main()
{
    int n;
    cout << "Enter number of productions: ";
    cin >> n;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    vector<string> temp_prod;
    string input;
    for (int i = 0; i < n; i++)
    {
        cout << "Enter Production " << i + 1 << ": ";
        getline(cin, input);
        temp_prod.push_back(input);
    }
    for (int i = 0; i < n; i++)
    {
        // cout<<temp_prod[i]<<endl;
        createStruct(temp_prod[i]);
    }
    cout << "Productions" << endl;
    for (struct NonTerminal temp_p : p)
    {
        cout << temp_p.nt << " -> ";
        for (int i = 0; i < temp_p.rhs.size(); i++)
        {
            cout << temp_p.rhs[i];
            if (i != temp_p.rhs.size() - 1)
            {
                cout << '|';
            }
        }
        cout << endl;
    }
    cout << "\nLeading:" << endl;
    for (auto &temp_p : p)
    {
        temp_p.lead = getLeading(temp_p.nt);
        cout << temp_p.nt << ":{ ";
        for (char c : temp_p.lead)
        {
            cout << c << " ";
        }
        cout << "}" << endl;
    }
    cout << "\nTrailing:" << endl;
    for (auto &temp_p : p)
    {
        temp_p.trail = getTrailing(temp_p.nt);
        cout << temp_p.nt << ":{ ";
        for (char c : temp_p.trail)
        {
            cout << c << " ";
        }
        cout << "}" << endl;
    }
    return 0;
}