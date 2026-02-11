/* Write a LEX program to recognize relational and logical operators. */

%{
    #include <stdio.h>
%}

relational "<="|">="|"=="|"!="|"<"|">"
logical "&&"|"||"|"!"

%%
{relational}    {printf("RELATIONAL OPERATOR: %s.\n", yytext);}
{logical}       {printf("LOGICAL OPERATOR: %s.\n", yytext);}
.|\n            {}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Inputs here:\n");
    yylex();
    return 0;
}