#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <stack>
#include <algorithm>
using namespace std;

string infix_to_postfix(string infix)
{
    string pf = "";
    stack<char> op_stack;
    for (char c : infix)
    {
        if (c != '*' && c != '|' && c != '.' && c != '(' && c != ')')
        {
            pf += c;
        }
        else if (c == '(')
        {
            op_stack.push(c);
        }
        else if (c == ')')
        {
            while (!op_stack.empty() && op_stack.top() != '(')
            {
                pf += op_stack.top();
                op_stack.pop();
            }
            op_stack.pop();
        }
        else
        {
            while (!op_stack.empty() &&
                   ((c == '*' && (op_stack.top() == '*')) ||
                    (c == '.' && (op_stack.top() == '*' || op_stack.top() == '.')) ||
                    (c == '|' && (op_stack.top() == '*' || op_stack.top() == '.' || op_stack.top() == '|'))))
            {
                pf += op_stack.top();
                op_stack.pop();
            }
            op_stack.push(c);
        }
    }
    while (!op_stack.empty())
    {
        pf += op_stack.top();
        op_stack.pop();
    }
    return pf;
}

vector<int> operator+(vector<int> a, vector<int> b)
{
    a.insert(a.end(), b.begin(), b.end());
    sort(a.begin(), a.end());
    a.erase(unique(a.begin(), a.end()), a.end());
    return a;
}

class tree
{
public:
    char c;
    int lno;
    vector<int> first, last;
    bool nullable;
    tree *left;
    tree *right;

    tree(char ch, int n)
    {
        c = ch;
        lno = n;
        nullable = false;
        left = nullptr;
        right = nullptr;
        first.clear();
        last.clear();
    }
};

int main()
{
    string rx;
    cout << "Enter a Regular Expression in infix form: ";
    cin >> rx;
    rx = "(" + rx + ").#";
    string postfix = infix_to_postfix(rx);
    cout << "Postfix Expression: " << postfix << endl
         << endl;

    vector<vector<int>> follow;
    vector<char> chars;
    set<char> inputs;
    stack<tree *> st;
    int leafno = 0;
    tree *temp;

    for (int i = 0; i < postfix.size(); i++)
    {
        char t = postfix[i];
        if (t != '*' && t != '.' && t != '|')
        {
            cout << "Character at leaf node number " << leafno << " is: " << t << ". ";
            chars.push_back(t);
            if (t != '#')
                inputs.insert(t);
            temp = new tree(t, leafno);
            temp->first.push_back(leafno);
            temp->last.push_back(leafno);

            follow.push_back(vector<int>());
            cout << "This node isn't nullable." << endl;
            cout << "Firstpos: {" << leafno << "}." << endl;
            cout << "Lastpos: {" << leafno << "}." << endl
                 << endl;

            leafno++;
        }
        else if (t == '*')
        {
            cout << "This isn't a leaf node. It contains the character: '*' (star). It is nullable." << endl;
            temp = new tree(t, -1);
            temp->left = st.top();
            st.pop();
            temp->first = temp->left->first;
            temp->last = temp->left->last;
            temp->nullable = true;
            cout << "Firstpos: {";
            for (int j = 0; j < temp->first.size(); j++)
            {
                cout << temp->first[j] << ", ";
            }
            cout << "}." << endl;
            cout << "Lastpos: {";
            for (int j = 0; j < temp->last.size(); j++)
            {
                cout << temp->last[j] << ", ";
                follow[temp->last[j]] = follow[temp->last[j]] + temp->first;
            }
            cout << "}." << endl
                 << endl;
        }
        else if (t == '.')
        {
            cout << "This isn't a leaf node. It contains the character: '.' (concatenation). ";
            temp = new tree(t, -1);
            temp->right = st.top();
            st.pop();
            temp->left = st.top();
            st.pop();
            temp->nullable = temp->left->nullable && temp->right->nullable;
            if (temp->nullable)
                cout << "The node is nullable." << endl;
            else
                cout << "The node isn't nullable." << endl;

            if (temp->left->nullable)
                temp->first = temp->left->first + temp->right->first;
            else
                temp->first = temp->left->first;
            cout << "Firstpos: {";
            for (int j = 0; j < temp->first.size(); j++)
            {
                cout << temp->first[j] << ", ";
            }
            cout << "}." << endl;

            if (temp->right->nullable)
                temp->last = temp->right->last + temp->left->last;
            else
                temp->last = temp->right->last;
            cout << "Lastpos: {";
            for (int j = 0; j < temp->last.size(); j++)
            {
                cout << temp->last[j] << ", ";
            }
            cout << "}." << endl
                 << endl;

            for (int j = 0; j < temp->left->last.size(); j++)
            {
                follow[temp->left->last[j]] = follow[temp->left->last[j]] + temp->right->first;
            }
        }
        else if (t == '|')
        {
            cout << "This isn't a leaf node. It contains the character: '|' (or). ";
            temp = new tree(t, -1);
            temp->right = st.top();
            st.pop();
            temp->left = st.top();
            st.pop();
            temp->nullable = temp->left->nullable || temp->right->nullable;
            if (temp->nullable)
                cout << "The node is nullable." << endl;
            else
                cout << "The node isn't nullable." << endl;

            temp->first = temp->left->first + temp->right->first;
            cout << "Firstpos: {";
            for (int j = 0; j < temp->first.size(); j++)
            {
                cout << temp->first[j] << ", ";
            }
            cout << "}." << endl;

            temp->last = temp->right->last + temp->left->last;
            cout << "Lastpos: {";
            for (int j = 0; j < temp->last.size(); j++)
            {
                cout << temp->last[j] << ", ";
            }
            cout << "}." << endl
                 << endl;
        }
        st.push(temp);
    }
    cout << endl;
    for (int i = 0; i < leafno; i++)
    {
        cout << "Leaf number: " << i << ". ";
        cout << "Character: " << chars[i] << "." << endl;
        cout << "Followpos: {";
        for (int j = 0; j < follow[i].size(); j++)
            cout << follow[i][j] << ", ";
        cout << "}." << endl
             << endl;
    }
    return 0;
}