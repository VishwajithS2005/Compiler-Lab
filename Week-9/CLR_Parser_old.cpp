/*
 ============================================================
  CLR(1) Parser for the Grammar:
    (0) S' -> S
    (1) S  -> C C
    (2) C  -> c C
    (3) C  -> d

  Input string: ccdd
 ============================================================

  Canonical LR(1) Item Sets
  ─────────────────────────
  I0:  S' -> .S  [$]
       S  -> .CC  [$]
       C  -> .cC  [c,d]
       C  -> .d   [c,d]

  I1 (I0 --S-->):  S' -> S.  [$]               → ACCEPT

  I2 (I0 --C-->):  S  -> C.C  [$]
                   C  -> .cC  [$]
                   C  -> .d   [$]

  I3 (I0 --c-->):  C  -> c.C  [c,d]
                   C  -> .cC  [c,d]
                   C  -> .d   [c,d]

  I4 (I0 --d-->):  C  -> d.   [c,d]            → reduce by C->d

  I5 (I2 --C-->):  S  -> CC.  [$]              → reduce by S->CC

  I6 (I2 --c-->):  C  -> c.C  [$]
                   C  -> .cC  [$]
                   C  -> .d   [$]

  I7 (I2 --d-->):  C  -> d.   [$]              → reduce by C->d

  I8 (I3 --C-->):  C  -> cC.  [c,d]            → reduce by C->cC

  I9 (I6 --C-->):  C  -> cC.  [$]              → reduce by C->cC
     (I3 --c--> I3,  I3 --d--> I4)
     (I6 --c--> I6,  I6 --d--> I7)

 ============================================================
  CLR(1) Parsing Table
  ─────────────────────────────────────────────────────────
  State │  ACTION         │  GOTO
        │  c    d    $    │  S   C
  ──────┼─────────────────┼───────
    0   │ s3   s4    -    │  1   2
    1   │  -    -   acc   │  -   -
    2   │ s6   s7    -    │  -   5
    3   │ s3   s4    -    │  -   8
    4   │ r3   r3    -    │  -   -
    5   │  -    -   r1    │  -   -
    6   │ s6   s7    -    │  -   9
    7   │  -    -   r3    │  -   -
    8   │ r2   r2    -    │  -   -
    9   │  -    -   r2    │  -   -
  ─────────────────────────────────────────────────────────
  sN = shift to state N
  rN = reduce by production N
  r1: S->CC  (pop 2*2=4, push S, goto[top][S])
  r2: C->cC  (pop 2*2=4, push C, goto[top][C])
  r3: C->d   (pop 1*2=2, push C, goto[top][C])
 ============================================================
*/

#include <iostream>
#include <stack>
#include <string>
#include <iomanip>
#include <vector>

using namespace std;

// ── Table entries ──────────────────────────────────────────
// ACTION table: action[state][terminal]
//   >0  → shift to state n
//   <0  → reduce by production |n|
//    0  → error
//   99  → accept
// terminal index: 0='c', 1='d', 2='$'

const int STATES = 10;
const int TERMINALS = 3;    // c  d  $
const int NONTERMINALS = 2; // S  C

int action[STATES][TERMINALS] = {
    //       c    d    $
    /* 0 */ {3, 4, 0},
    /* 1 */ {0, 0, 99}, // 99 = ACCEPT
    /* 2 */ {6, 7, 0},
    /* 3 */ {3, 4, 0},
    /* 4 */ {-3, -3, 0}, // reduce C->d
    /* 5 */ {0, 0, -1},  // reduce S->CC
    /* 6 */ {6, 7, 0},
    /* 7 */ {0, 0, -3},  // reduce C->d
    /* 8 */ {-2, -2, 0}, // reduce C->cC
    /* 9 */ {0, 0, -2}   // reduce C->cC
};

// GOTO table: gotoTable[state][non-terminal]
// non-terminal index: 0=S, 1=C
int gotoTable[STATES][NONTERMINALS] = {
    //       S   C
    /* 0 */ {1, 2},
    /* 1 */ {0, 0},
    /* 2 */ {0, 5},
    /* 3 */ {0, 8},
    /* 4 */ {0, 0},
    /* 5 */ {0, 0},
    /* 6 */ {0, 9},
    /* 7 */ {0, 0},
    /* 8 */ {0, 0},
    /* 9 */ {0, 0}};

// Production rules: {LHS non-terminal index, RHS length}
struct Production
{
    int lhs;
    int rhsLen;
    string name;
};
Production prods[] = {
    {0, 0, ""},         // dummy [0] – not used
    {0, 2, "S -> C C"}, // prod 1
    {1, 2, "C -> c C"}, // prod 2
    {1, 1, "C -> d"}    // prod 3
};

// ── Helpers ────────────────────────────────────────────────
int termIndex(char c)
{
    if (c == 'c')
        return 0;
    if (c == 'd')
        return 1;
    if (c == '$')
        return 2;
    return -1;
}

string termName(int idx)
{
    string t[] = {"c", "d", "$"};
    return t[idx];
}

string ntName(int idx)
{
    return idx == 0 ? "S" : "C";
}

// ── Pretty print helper ────────────────────────────────────
void printTable()
{
    cout << "              CLR(1) PARSING TABLE                      \n";
    cout << "  State      ACTION                    GOTO             \n";
    cout << "           c      d      $         S        C           \n";

    auto fmtA = [](int v) -> string
    {
        if (v == 0)
            return "  -  ";
        if (v == 99)
            return " acc ";
        if (v > 0)
            return "  s" + to_string(v) + "  ";
        return "  r" + to_string(-v) + "  ";
    };
    auto fmtG = [](int v) -> string
    {
        return v == 0 ? "   -   " : "   " + to_string(v) + "   ";
    };

    for (int s = 0; s < STATES; s++)
    {
        cout << "    " << s << "    ";
        for (int t = 0; t < TERMINALS; t++)
            cout << setw(5) << fmtA(action[s][t]) << "  ";
        cout << " ";
        for (int n = 0; n < NONTERMINALS; n++)
            cout << setw(6) << fmtG(gotoTable[s][n]) << " ";
        cout << " \n";
    }
}

// ── Grammar & productions display ─────────────────────────
void printGrammar()
{
    cout << "            GRAMMAR                 \n";
    cout << "   (0) S' -> S                      \n";
    cout << "   (1) S  -> C C                    \n";
    cout << "   (2) C  -> c C                    \n";
    cout << "   (3) C  -> d                      \n";
}

// ── Main parser ────────────────────────────────────────────
void parse(const string &input)
{
    string inp = input + "$";
    int ip = 0;

    stack<int> stateStk;
    stack<string> symStk;
    stateStk.push(0);

    cout << "                         PARSING TRACE                                     \n";
    cout << "   Step     Stack (states)     Symbols               Input    Action       \n";

    int step = 0;
    bool accepted = false;
    bool error = false;

    // helper to print current stack
    auto stackStr = [&]() -> string
    {
        // copy stack to vector
        stack<int> tmp = stateStk;
        vector<int> v;
        while (!tmp.empty())
        {
            v.push_back(tmp.top());
            tmp.pop();
        }
        string s;
        for (int i = v.size() - 1; i >= 0; i--)
            s += to_string(v[i]) + " ";
        return s;
    };
    auto symStr = [&]() -> string
    {
        stack<string> tmp = symStk;
        vector<string> v;
        while (!tmp.empty())
        {
            v.push_back(tmp.top());
            tmp.pop();
        }
        string s = "$";
        for (int i = 0; i < (int)v.size(); i++)
            s += v[i];
        return s;
    };

    while (true)
    {
        step++;
        int state = stateStk.top();
        char ch = inp[ip];
        int ti = termIndex(ch);

        string stStr = stackStr();
        string syStr = symStr();
        string inpRem = inp.substr(ip);
        int act = (ti >= 0) ? action[state][ti] : 0;

        string actStr;
        if (act == 99)
            actStr = "ACCEPT";
        else if (act > 0)
            actStr = "Shift  -> s" + to_string(act);
        else if (act < 0)
            actStr = "Reduce -> r" + to_string(-act) + " (" + prods[-act].name + ")";
        else
            actStr = "ERROR";

        cout << "  " << setw(6) << step << "   "
             << setw(16) << left << stStr << "   "
             << setw(19) << left << syStr << "   "
             << setw(6) << left << inpRem << "    "
             << setw(12) << left << actStr << "  \n";

        if (act == 99)
        {
            accepted = true;
            break;
        }
        if (act == 0)
        {
            error = true;
            break;
        }

        if (act > 0)
        {
            // SHIFT
            stateStk.push(act);
            symStk.push(string(1, ch));
            ip++;
        }
        else
        {
            // REDUCE
            int prod = -act;
            int rLen = prods[prod].rhsLen;
            for (int i = 0; i < rLen; i++)
            {
                stateStk.pop();
                symStk.pop();
            }
            int top = stateStk.top();
            int lhs = prods[prod].lhs;
            int next = gotoTable[top][lhs];
            if (next == 0)
            {
                error = true;
                break;
            }
            stateStk.push(next);
            symStk.push(ntName(lhs));
        }
    }

    if (accepted)
        cout << "✅  String \"" << input << "\" is ACCEPTED by the grammar.\n\n";
    else
        cout << "❌  String \"" << input << "\" is REJECTED (syntax error).\n\n";
}

// ── Entry point ────────────────────────────────────────────
int main()
{
    cout << "\n";
    printGrammar();
    printTable();

    // Test with ccdd
    cout << "  Parsing input: ccdd\n";
    parse("ccdd");

    // Bonus: test an invalid string
    cout << "  Parsing input: ccd  (invalid)\n";
    parse("ccd");

    return 0;
}