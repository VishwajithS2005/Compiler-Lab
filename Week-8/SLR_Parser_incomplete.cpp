#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

struct aug_gram_ele
{
    char LHS;
    string RHS;
};

struct goto_gram_ele
{
    char LHS;
    string RHS;
    int cursor;
};

bool is_non_terminal(char c)
{
    return isupper(c);
}

int main()
{
    cout << "Enter the number of total productions: ";
    int n;
    cin >> n;
    vector<aug_gram_ele> aug_gram;
    for (int i = 0; i <= n; i++)
    {
        if (i == 0)
        {
            aug_gram_ele e;
            cout << endl
                 << "Choose the symbol for the start of the grammar: ";
            cin >> e.LHS;
            cout << "Enter the symbol that's the actual start of the grammar: ";
            cin >> e.RHS;
            aug_gram.push_back(e);
        }
        else
        {
            int j;
            cout << endl
                 << "Enter the number of productions for the non-terminal: ";
            cin >> j;
            char LHS;
            cout << "Enter the LHS: ";
            cin >> LHS;
            for (int k = 0; k < j; k++)
            {
                aug_gram_ele e;
                e.LHS = LHS;
                cout << "Enter the production: ";
                cin >> e.RHS;
                aug_gram.push_back(e);
            }
        }
    }
    cout << endl
         << "The augmented grammar is:" << endl;
    for (auto i : aug_gram)
    {
        cout << i.LHS << " -> " << i.RHS << endl;
    }
    return 0;
}