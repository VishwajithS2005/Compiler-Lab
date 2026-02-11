/* Write a LEX program to count the number of operators and operands. */

%{
    #include <stdio.h>
    int operators = 0;
    int operands = 0;
%}

operator "<<"|">>"|"&&"|"||"|"<="|">="|"=="|"++"|"--"|"+"|"-"|"*"|"/"|"%"|"<"|">"|"="|"&"|"|"|"^"
operand [a-zA-Z_][a-zA-Z0-9_]*
whitespace [ \t\n]+

%%
{operator}      {operators++;}
{operand}       {operands++;}
{whitespace}    {}
.               {}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Inputs here:\n");
    yylex();
    printf("Number of operands: %d.\n", operands);
    printf("Number of operators: %d.\n", operators);
    return 0;
}