#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

vector<string> keywords = {"program", "var", "integer", "begin", "while", "do", "if", "then", "else", "end"};
vector<string> operators = {":", "<>", ">", ":=", "-"};
vector<string> punctuators = {"(", ")", ",", ";"};

bool hasPunctuator(string word)
{
    for (const auto &punct : punctuators)
    {
        if (word.find(punct) != string::npos)
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
        for (const auto &punct : punctuators)
        {
            size_t pos;
            while ((pos = word.find(punct)) != string::npos)
            {
                string beforePunct = word.substr(0, pos);
                if (!beforePunct.empty())
                {
                    printToken(beforePunct);
                }
                cout << "Type: PUNCTUATOR, Value: " << punct << endl;
                word = word.substr(pos + punct.length());
            }
        }
        if (!word.empty())
        {
            printToken(word);
        }
        return;
    }

    auto itk = find(keywords.begin(), keywords.end(), word);
    auto ito = find(operators.begin(), operators.end(), word);
    auto itp = find(punctuators.begin(), punctuators.end(), word);

    if (itk != keywords.end())
    {
        cout << "Type: KEYWORD, Value: " << word << endl;
    }
    else if (ito != operators.end())
    {
        cout << "Type: OPERATOR, Value: " << word << endl;
    }
    else if (itp != punctuators.end())
    {
        cout << "Type: PUNCTUATOR, Value: " << word << endl;
    }
    else
    {
        cout << "Type: MODIFIER, Value: " << word << endl;
    }
}

int main()
{
    ifstream inputFile("Code.txt");
    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open the file 'Code.txt'" << endl;
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