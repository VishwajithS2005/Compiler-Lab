/* Write a LEX program to count the number of lines in a file. */

%{
    #include <stdio.h>
    int lines = 0;
%}

%%
.   {}
\n  {lines++;}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Input here: ");
    yylex();
    printf("The number of lines is: %d.\n", lines);
    return 0;
}