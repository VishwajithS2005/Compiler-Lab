#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

vector<string> keywords = {"include", "using", "namespace", "int", "if", "return"};
vector<string> operators = {"<", ">", "=", "-", ">>", "<<"};
vector<string> special_symbols = {"(", ")", ";", "{", "}", "#"};

bool hasPunctuator(string word)
{
    for (const auto &spec : special_symbols)
    {
        if (word.find(spec) != string::npos)
        {
            return true;
        }
    }
    return false;
}

void printToken(string word)
{
    if (word.length() > 1 && hasPunctuator(word))
    {
        for (const auto &spec : special_symbols)
        {
            size_t pos;
            while ((pos = word.find(spec)) != string::npos)
            {
                string beforePunct = word.substr(0, pos);
                if (!beforePunct.empty())
                {
                    printToken(beforePunct);
                }
                cout << "Type: SPECIAL SYMBOL, Value: " << spec << endl;
                word = word.substr(pos + spec.length());
            }
        }
        if (!word.empty())
        {
            printToken(word);
        }
        return;
    }

    if (word.find("<") != string::npos && word.find(">") != string::npos && word.length() > 1)
    {
        cout << "Type: OPERATOR, Value: <" << endl;
        word.erase(remove(word.begin(), word.end(), '<'), word.end());
        word.erase(remove(word.begin(), word.end(), '>'), word.end());
        printToken(word);
        cout << "Type: OPERATOR, Value: >" << endl;
        return;
    }

    auto itk = find(keywords.begin(), keywords.end(), word);
    auto ito = find(operators.begin(), operators.end(), word);
    auto itp = find(special_symbols.begin(), special_symbols.end(), word);

    if (itk != keywords.end())
    {
        cout << "Type: KEYWORD, Value: " << word << endl;
    }
    else if (ito != operators.end())
    {
        cout << "Type: OPERATOR, Value: " << word << endl;
    }
    else if (itp != special_symbols.end())
    {
        cout << "Type: PUNCTUATOR, Value: " << word << endl;
    }
    else
    {
        cout << "Type: IDENTIFIER, Value: " << word << endl;
    }
}

int main()
{
    ifstream inputFile("Factorial.cpp");
    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open the file 'Factorial.cpp'" << endl;
        return 1;
    }

    string word;
    while (inputFile >> word)
    {
        printToken(word);
    }
    inputFile.close();
    return 0;
}