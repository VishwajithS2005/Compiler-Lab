/* Write a LEX program to convert all lowercase letters to uppercase. */
%{
#include <stdio.h>
#include <ctype.h>
%}

%%
[a-z]      { yytext[0] = toupper(yytext[0]); ECHO; }
.|\n       { ECHO; }
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Input here: ");
    fflush(stdout);
    yylex();
    return 0;
}
