/* Write a LEX program to count the number of uppercase and lowercase letters. */

%{
    #include <stdio.h>
    int upper = 0;
    int lower = 0;
%}

%%
[a-z]   {lower++;}
[A-Z]   {upper++;}
.|\n    {}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Input here:");
    yylex();
    printf("Number of uppercase letters is: %d.\n", upper);
    printf("Number of lowercase letters is: %d.\n", lower);
    return 0;
}