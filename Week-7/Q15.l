/* Write a LEX program to perform lexical analysis of a C program. */

%{
    #include <stdio.h>
    #include <string.h>
%}

keyword     int|float|char|double|if|else|while|for|return|void|break|continue
id          [a-zA-Z_][a-zA-Z0-9_]*
number      [0-9]+(\.[0-9]+)?
operator    "=="|"!="|"<="|">="|"++"|"--"|"+"|"-"|"*"|"/"|"="|"<"|">"
delimiter   [;(),{}]

%%
{keyword}   {printf("Keyword: %s\n", yytext);}
{id}        {printf("Identifier: %s\n", yytext);}
{number}    {printf("Constant: %s\n", yytext);}
{operator}  {printf("Operator: %s\n", yytext);}
{delimiter} {printf("Delimiter: %s\n", yytext);}
[ \t\n]+    {}
.           {printf("Unknown: %s\n", yytext);}
%%

int yywrap(){
    return 1;
}

int main(){
    printf("Enter the C code here:\n");
    yylex();
    return 0;
}