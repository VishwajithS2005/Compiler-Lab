/* Write a LEX program to find and print all email IDs from a given text. */

%{
    #include <stdio.h>
%}

email [a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}

%%
{email} {printf("Email found: %s\n", yytext);}

.|\n    {}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Inputs here:\n\n");
    yylex();
    return 0;
}
