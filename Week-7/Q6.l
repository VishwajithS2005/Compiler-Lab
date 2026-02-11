/* Write a LEX program to identify and count digits in the input. */

%{
    #include <stdio.h>
    int digits = 0;
%}

%%
[0-9] {digits++;}
.|\n {}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Input here: ");
    yylex();
    printf("The number of digits is: %d.\n", digits);
    return 0;
}