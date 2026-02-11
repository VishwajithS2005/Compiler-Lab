/* Write a LEX program to validate arithmetic expression */

%{
    #include <stdio.h>
    int valid = 1;
    int paren = 0;
%}

%%
[0-9]+                  {}
[a-zA-Z_][a-zA-Z0-9_]*  {}
"("                     {paren++;}
")"                     {paren--;if(paren<0) valid=0;}
[+\-*/]                 {}
[ \t\n]+                {}
.                       {valid = 0;}
%%

int yywrap(){
    return 1;
}

int main(){
    printf("Enter expression:\n");
    yylex();
    if(valid && paren==0)
        printf("Valid Arithmetic Expression\n");
    else
        printf("Invalid Arithmetic Expression\n");
    return 0;
}
