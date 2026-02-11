/* Write a LEX program to recognize integers and floating-point numbers. */

%{
    #include <stdio.h>
%}

%%
[0-9]+[.][0-9]+ {printf("Floating point value.\n");}
[0-9]+          {printf("Integer.\n");}
.|\n            {}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Input here:\n");
    yylex();
    return 0;
}