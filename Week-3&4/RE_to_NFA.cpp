#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <map>
using namespace std;

map<int, vector<pair<char, int>>> get_transition_table()
{
    map<int, vector<pair<char, int>>> tt;

    tt[0].push_back({'a', 0});
    tt[0].push_back({'b', 0});
    tt[0].push_back({'e', 1});
    tt[1].push_back({'a', 2});
    tt[2].push_back({'a', 3});
    tt[2].push_back({'b', 3});

    return tt;
}

int get_next_state(vector<pair<char, int>> transitions, char symbol)
{
    for (auto &[c, i] : transitions)
    {
        if (c == symbol)
            return i;
    }
    return -1;
}

void null_closure(set<int> &states, map<int, vector<pair<char, int>>> tt)
{
    bool changed;
    do
    {
        changed = false;
        set<int> snapshot = states;
        for (int s : snapshot)
        {
            int e = get_next_state(tt[s], 'e');
            if (e != -1 && !states.count(e))
            {
                states.insert(e);
                changed = true;
            }
        }
    } while (changed);
}

int main()
{
    string s;
    cout << "Enter a string: ";
    cin >> s;

    map<int, vector<pair<char, int>>> tt = get_transition_table();
    cout << endl
         << "Current State\tSymbol\tNew State" << endl;
    for (auto &[old_state, transitions] : tt)
    {
        for (auto &[symbol, new_state] : transitions)
        {
            cout << old_state << "\t\t" << symbol << "\t\t" << new_state << endl;
        }
    }
    cout << endl;

    set<int> states = {0};
    null_closure(states, tt);
    for (char c : s)
    {
        set<int> next_states;
        for (int st : states)
        {
            int ns = get_next_state(tt[st], c);
            if (ns != -1)
                next_states.insert(ns);
        }

        states = next_states;
        null_closure(states, tt);
    }

    for (int i : states)
    {
        if (i == 3)
        {
            cout << "The given string is accepted." << endl;
            return 0;
        }
    }

    cout << "The given string is rejected." << endl;
    return 0;
}