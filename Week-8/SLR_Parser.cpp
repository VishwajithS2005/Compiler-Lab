#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
using namespace std;

struct prod
{
    int lhsIdx;
    vector<pair<int, int>> rhsIdx; // nt/t, nt/tIdx - char
};
struct stackItem
{
    int type; // 0-state, 1-NT, 2-T
    int state;
    int ntIdx;
    int tIdx;
};

char startSym;
int startSymIdx;
set<char> terms, nterms;
vector<prod> prods;
vector<pair<int, int>> litems; // prodIdx, .pos - lr0 item
vector<vector<int>> lstates;   // stateNo - vector(litemsIdx)
vector<vector<int>> closures;  // ntIdx - vector(litemsIdx)
vector<int> cproc;
vector<int> fiproc, foproc;
vector<set<char>> first, follow;           // ntIdx - set(first) and set(follow)
map<pair<int, pair<int, int>>, int> gotos; // stateNo, (nt/t nt/tIdx) -> stateNo
vector<vector<string>> table;              // stateNo, t-ntIdx - Action
string input;
vector<stackItem> stack;

int findState(vector<int> v)
{
    set<int> state(v.begin(), v.end());
    for (int i = 0; i < lstates.size(); i++)
    {
        set<int> cstate(lstates[i].begin(), lstates[i].end());
        if (state == cstate)
            return i;
    }
    return -1;
}

int findLitem(pair<int, int> litem, vector<pair<int, int>> lritems)
{
    for (int i = 0; i < lritems.size(); i++)
    {
        if (litem == lritems[i])
            return i;
    }
    return -1;
}

int findInt(int i, vector<int> v)
{
    for (int j = 0; j < v.size(); j++)
    {
        if (i == v[j])
            return j;
    }
    return -1;
}

set<char> fillFirst(char t)
{
    int ntIdx = distance(nterms.begin(), nterms.find(t));
    if (fiproc[ntIdx])
        return first[ntIdx];
    fiproc[ntIdx] = 1;
    for (prod p : prods)
    {
        if (p.lhsIdx == ntIdx)
        {
            int fnt = p.rhsIdx[0].first;
            int fIdx = p.rhsIdx[0].second;
            if (fnt)
                first[ntIdx].insert(*next(terms.begin(), fIdx));
            else
            {
                char tgt = *next(nterms.begin(), fIdx);
                for (char c : fillFirst(tgt))
                    first[ntIdx].insert(c);
            }
        }
    }
    return first[ntIdx];
}

set<char> fillFollow(char t)
{
    int ntIdx = distance(nterms.begin(), nterms.find(t));
    if (foproc[ntIdx])
        return follow[ntIdx];
    foproc[ntIdx] = 1;
    for (prod p : prods)
    {
        for (int i = 0; i < p.rhsIdx.size(); i++)
        {
            int fnt = p.rhsIdx[i].first;
            int fntIdx = p.rhsIdx[i].second;
            if (fnt == 0 && fntIdx == ntIdx)
            {
                if (i == p.rhsIdx.size() - 1)
                {
                    char tgt = *next(nterms.begin(), p.lhsIdx);
                    for (char c : fillFollow(tgt))
                        follow[ntIdx].insert(c);
                }
                else
                {
                    int fn = p.rhsIdx[i + 1].first;
                    int fnIdx = p.rhsIdx[i + 1].second;
                    if (fn)
                        follow[ntIdx].insert(*next(terms.begin(), fnIdx));
                    else
                    {
                        char tgt = *next(nterms.begin(), fnIdx);
                        for (char c : fillFirst(tgt))
                            follow[ntIdx].insert(c);
                    }
                }
            }
        }
    }

    return follow[ntIdx];
}

vector<int> findClosure(int nt)
{
    if (cproc[nt])
        return closures[nt];
    cproc[nt] = 1;
    vector<int> litems;
    for (int i = 0; i < prods.size(); i++)
    {
        if (prods[i].lhsIdx == nt)
        {
            litems.push_back(findLitem(pair<int, int>(i, 0), ::litems));
            if (prods[i].rhsIdx[0].first == 0 &&
                prods[i].rhsIdx[0].second != nt)
            {
                vector<int> vc = findClosure(prods[i].rhsIdx[0].second);
                for (int i : vc)
                {
                    if (findInt(i, litems) == -1)
                        litems.push_back(i);
                }
            }
        }
    }
    closures[nt] = litems;
    return litems;
}

void constructClosures()
{
    for (int i = 0; i < prods.size(); i++)
        litems.push_back(pair<int, int>(i, 0));
    cproc = vector<int>(nterms.size(), 0);
    closures = vector<vector<int>>(nterms.size());
    for (int i = 0; i < nterms.size(); i++)
        findClosure(i);
}

void printProd(prod pro, int i)
{
    char l = *next(nterms.begin(), pro.lhsIdx);
    if (l == 'Z')
        cout << startSym << "'" << "->";
    else
        cout << l << "->";
    int x = 0;
    for (pair<int, int> ridx : pro.rhsIdx)
    {
        char c;
        if (ridx.first == 0)
            c = *next(nterms.begin(), ridx.second);
        else
            c = *next(terms.begin(), ridx.second);
        if (x++ == i)
            cout << ".";
        cout << c;
    }

    if (i == pro.rhsIdx.size())
        cout << ".";
    cout << endl;
}

void findGoto(int lstateNo, char c)
{

    int nt = 1;
    if (nterms.count(c))
        nt = 0;
    int ntIdx;
    if (nt == 0)
        ntIdx = distance(nterms.begin(), nterms.find(c));
    else
        ntIdx = distance(terms.begin(), terms.find(c));
    vector<int> lritems;
    int found = 0;
    for (int x : lstates[lstateNo])
    {
        pair<int, int> p = litems[x];
        prod pr = prods[p.first];
        int pos = p.second;
        if (pr.rhsIdx.size() > pos)
        {
            if (pair<int, int>(nt, ntIdx) == pr.rhsIdx[pos])
            {
                found = 1;
                pair<int, int> newitem(p.first, pos + 1);
                int newIdx = findLitem(newitem, litems);
                if (newIdx == -1)
                {
                    litems.push_back(newitem);
                    lritems.push_back(litems.size() - 1);
                }
                else
                    lritems.push_back(newIdx);
                if (pos + 1 < pr.rhsIdx.size())
                {
                    pair<int, int> clositem = pr.rhsIdx[pos + 1];
                    if (clositem.first == 0)
                    {
                        vector<int> itemclos = closures[clositem.second];
                        for (int i : itemclos)
                        {
                            if (findInt(i, lritems) == -1)
                                lritems.push_back(i);
                        }
                    }
                }
            }
        }
    }
    pair<int, int> nti(nt, ntIdx);
    pair<int, pair<int, int>> gt(lstateNo, nti);
    int gtstatenum = -1;
    if (!found)
    {
        pair<pair<int, pair<int, int>>, int> val(gt, gtstatenum);
        gotos.insert(val);
    }
    else if (findState(lritems) == -1)
    {
        lstates.push_back(lritems);
        gtstatenum = lstates.size() - 1;
    }
    else
        gtstatenum = findState(lritems);
    pair<pair<int, pair<int, int>>, int> val(gt, gtstatenum);
    gotos.insert(val);
}

void printGotos()
{
    int x = 0;
    int s = nterms.size() + terms.size() - 1;
    for (pair<pair<int, pair<int, int>>, int> p : gotos)
    {
        int nt = p.first.second.first;
        int ntIdx = p.first.second.second;
        char ntc;
        if (nt)
            ntc = *next(terms.begin(), ntIdx);
        else
            ntc = *next(nterms.begin(), ntIdx);
        int gst = p.second;
        if (gst != -1)
            cout << "goto(" << p.first.first << "," << ntc << "): " << gst << " | ";
        else
            cout << "goto(" << p.first.first << "," << ntc << "): - | ";
        x = (x + 1) % s;
        if (x == 0)
            cout << endl;
    }
}

void printClosures()
{
    for (int i = 0; i < closures.size(); i++)
    {
        cout << *next(nterms.begin(), i) << ": " << endl;
        for (int p : closures[i])
        {
            pair<int, int> pr = *next(litems.begin(), p);
            prod pro = prods[pr.first];
            printProd(pro, pr.second);
        }
        cout << endl;
    }
}

void printStates()
{
    for (int j = 0; j < lstates.size(); j++)
    {
        cout << "I" << j << endl;
        for (int p : lstates[j])
        {
            pair<int, int> pr = *next(litems.begin(), p);
            prod pro = prods[pr.first];
            printProd(pro, pr.second);
        }
        cout << endl;
    }
}

void printFollows()
{
    for (int i = 0; i < follow.size(); i++)
    {
        char c = *next(nterms.begin(), i);
        cout << c << ": {";
        for (char x : follow[i])
            cout << x << " ";
        cout << "}" << endl;
    }
    cout << endl;
}

void fillTable()
{
    for (pair<pair<int, pair<int, int>>, int> p : gotos)
    {
        int gst = p.second;
        int sn = p.first.first;
        int nt = p.first.second.first;
        int ntIdx = p.first.second.second;
        if (gst == -1)
            continue;
        if (nt)
            table[sn][ntIdx] = "s" + to_string(gst);
        else if (startSymIdx != ntIdx)
            table[sn][ntIdx + terms.size()] = to_string(gst);
    }
    for (int i = 0; i < lstates.size(); i++)
    {
        for (int lritem : lstates[i])
        {
            int pos = litems[lritem].second;
            int prodIdx = litems[lritem].first;
            if (pos == prods[prodIdx].rhsIdx.size())
            {
                for (char c : terms)
                {
                    int cIdx = distance(terms.begin(), terms.find(c));
                    if (follow[prods[prodIdx].lhsIdx].count(c))
                    {
                        string fstring;
                        if (prodIdx == 0)
                            fstring = "ACC";
                        else
                            fstring = "r" + to_string(prodIdx);
                        if (table[i][cIdx] == "-")
                            table[i][cIdx] = fstring;
                    }
                }
            }
        }
    }
}

void printTable()
{
    cout << "SLR Parsing Table: " << endl;
    int nnt = nterms.size() - 1;
    int ntt = terms.size();
    cout << setw(5) << "STATE"
         << setw(4) << "|"
         << setw(ntt * 5) << "ACTION"
         << setw(ntt * 5) << "|"
         << setw(nnt * 2) << "GOTO"
         << setw(nnt * 2) << "|" << endl;
    cout << setw(5) << "-"
         << setw(4) << "|";
    for (char c : terms)
        cout << setw(5) << c << setw(5) << "|";
    for (char c : nterms)
    {
        if (c != 'Z')
            cout << setw(2) << c << setw(2) << "|";
    }
    cout << endl;
    for (int i = 0; i < table.size(); i++)
    {
        cout << setw(5) << i << setw(4) << "|";
        int j = 0;
        for (; j < terms.size(); j++)
            cout << setw(5) << table[i][j] << setw(5) << "|";
        for (; j < table[i].size() - 1; j++)
            cout << setw(2) << table[i][j] << setw(2) << "|";
        cout << endl;
    }
}

void printStack()
{
    string out;
    for (stackItem s : stack)
    {
        if (s.type == 0)
            out += to_string(s.state);
        else if (s.type == 1)
            out += *next(nterms.begin(), s.ntIdx);
        else if (s.type == 2)
            out += *next(terms.begin(), s.tIdx);
    }
    cout << setw(10) << out << setw(10) << "|";
}

void parse(string in)
{
    in = in + '$';
    int i = 0;
    int j = 0;
    int acc = 0;
    stackItem init;
    init.type = 0;
    init.state = 0;
    stack.push_back(init);
    cout << setw(10) << "STACK"
         << setw(10) << "|"
         << setw(10) << "INPUT"
         << setw(10) << "|"
         << setw(20) << "ACTION"
         << setw(20) << "|" << endl;
    while (i < in.size() && acc == 0)
    {
        printStack();
        cout << setw(10) << in.substr(i) << setw(10) << "|";
        int tIdx = distance(terms.begin(), terms.find(in[i]));
        string a = table[stack[j].state][tIdx];
        if (a[0] == 's')
        {
            j += 2;
            stackItem newItem;
            newItem.type = 2;
            newItem.tIdx = distance(terms.begin(), terms.find(in[i++]));
            stack.push_back(newItem);
            newItem.type = 0;
            newItem.state = stoi(a.substr(1));
            newItem.tIdx = -1;
            stack.push_back(newItem);
            cout << setw(20) << "SHIFT " + to_string(newItem.state)
                 << setw(20) << "|" << endl;
        }
        else if (a[0] == 'A')
        {
            cout << setw(20) << "ACCEPT"
                 << setw(20) << "|" << endl;
            exit(0);
        }
        else if (a[0] == 'r')
        {
            int prodIdx = stoi(a.substr(1));
            prod rprod = prods[prodIdx];
            vector<pair<int, int>> rIdx;
            int ntop = j - 2 * rprod.rhsIdx.size() + 1;
            if (ntop < 0)
            {
                cout << setw(20) << "ERROR"
                     << setw(20) << "|" << endl;
                exit(0);
            }
            for (int x = ntop; x <= j; x += 2)
            {
                stackItem s1 = stack[x];
                rIdx.push_back(
                    pair<int, int>(
                        s1.type - 1,
                        s1.type == 1 ? s1.ntIdx : s1.tIdx));
            }
            while (j >= ntop)
            {
                j--;
                stack.pop_back();
            }
            if (rIdx == rprod.rhsIdx)
            {
                stackItem s2;
                s2.type = 1;
                s2.ntIdx = rprod.lhsIdx;
                int gt = stoi(
                    table[stack[j].state][terms.size() + s2.ntIdx]);
                stackItem s3;
                s3.type = 0;
                s3.state = gt;
                stack.push_back(s2);
                stack.push_back(s3);
                j += 2;
                char l = *next(nterms.begin(), rprod.lhsIdx);
                string nout = string(1, l) + "->";
                for (pair<int, int> ridx : rprod.rhsIdx)
                {
                    char c;
                    if (ridx.first == 0)
                        c = *next(nterms.begin(), ridx.second);
                    else
                        c = *next(terms.begin(), ridx.second);
                    nout += c;
                }
                cout << setw(20) << "REDUCE " + nout
                     << setw(20) << "|" << endl;
            }
            else
            {
                cout << setw(30) << "REDUCE FAILED"
                     << setw(10) << "|" << endl;
                cout << "Stack RHS: ";
                for (pair<int, int> p : rIdx)
                {
                    char c;
                    if (p.first == 0)
                        c = *next(nterms.begin(), p.second);
                    else
                        c = *next(terms.begin(), p.second);
                    cout << c << " ";
                }
                cout << endl
                     << "Target RHS: ";
                for (pair<int, int> p : rprod.rhsIdx)
                {
                    char c;
                    if (p.first == 0)
                        c = *next(nterms.begin(), p.second);
                    else
                        c = *next(terms.begin(), p.second);
                    cout << c << " ";
                }
                exit(0);
            }
        }
        else
        {
            cout << setw(20) << "ERROR"
                 << setw(20) << "|" << endl;
            exit(0);
        }
    }
}

int main()
{
    int stlr = 0;
    string in, t;
    vector<pair<int, int>> v;
    vector<prod> tempProds;
    prod p;
    cout << "Enter terminals seperated by comma: ";
    cin >> in;
    for (int i = 0; i < in.size(); i += 2)
        terms.insert(in[i]);
    cout << "Enter non-terminals seperated by comma: ";
    cin >> in;
    for (int i = 0; i < in.size(); i += 2)
        nterms.insert(in[i]);
    terms.insert('$');
    cout << "Enter starting symbol: ";
    cin >> startSym;
    cout << "Enter productions seperated by |: ";
    nterms.insert('Z');
    startSymIdx = distance(nterms.begin(), nterms.find('Z'));
    cin >> in;
    for (int i = 0; i < in.size(); i++)
    {
        char c = in[i];
        if (c == '|')
        {
            p.rhsIdx = v;
            tempProds.push_back(p);
            p.rhsIdx.clear();
            v.clear();
            stlr = 0;
        }
        else if (c == '-')
        {
            stlr = 1;
            i++;
        }
        else if (stlr == 0)
            p.lhsIdx = distance(nterms.begin(), nterms.find(c));
        else if (terms.count(c))
            v.push_back(
                pair<int, int>(
                    1,
                    distance(terms.begin(), terms.find(c))));
        else if (nterms.count(c))
            v.push_back(
                pair<int, int>(
                    0,
                    distance(nterms.begin(), nterms.find(c))));
        else
            cout << "ERROR";
    }
    cout << "Enter input string: ";
    cin >> input;
    p.rhsIdx = v;
    tempProds.push_back(p);
    p.rhsIdx.clear();
    v.clear();
    p.lhsIdx = distance(nterms.begin(), nterms.find('Z'));
    v.push_back(
        pair<int, int>(
            0,
            distance(nterms.begin(), nterms.find(startSym))));
    p.rhsIdx = v;
    prods.push_back(p);
    for (prod pr : tempProds)
        prods.push_back(pr);
    cout << endl
         << "Augmented productions: " << endl;
    for (int i = 0; i < prods.size(); i++)
    {
        cout << i << ". ";
        printProd(prods[i], -1);
    }
    fiproc = vector<int>(nterms.size(), 0);
    foproc = vector<int>(nterms.size(), 0);
    first = vector<set<char>>(nterms.size());
    follow = vector<set<char>>(nterms.size());
    follow[startSymIdx].insert('$');
    for (char c : nterms)
        fillFollow(c);
    constructClosures();
    lstates.push_back(findClosure(startSymIdx));
    for (int i = 0; i < lstates.size(); i++)
    {
        for (char c : terms)
        {
            if (c != '$')
                findGoto(i, c);
        }
        for (char c : nterms)
        {
            if (c != 'Z')
                findGoto(i, c);
        }
    }
    cout << endl
         << "States: " << endl;
    printStates();
    cout << "Gotos: " << endl;
    printGotos();
    cout << endl
         << endl;
    cout << "Follow(): " << endl;
    printFollows();
    table = vector<vector<string>>(
        lstates.size(),
        vector<string>(nterms.size() + terms.size(), "-"));
    fillTable();
    printTable();
    cout << endl
         << "Parsing Input: " << endl;
    parse(input);
}

// S->L=R|S->R|L->*R|L->id|R->L