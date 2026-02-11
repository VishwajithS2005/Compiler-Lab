/* Write a LEX program to generate tokens for keywords, identifiers, constants, and operators. */

%{
    #include <stdio.h>
%}

%%
int|float|char|double|if|else|while|for|return|void {printf("<KEYWORD, %s>\n", yytext);}
[a-zA-Z_][a-zA-Z0-9_]*                              {printf("<ID, %s>\n", yytext);}
[0-9]+(\.[0-9]+)?                                   {printf("<CONST, %s>\n", yytext);}
"=="|"!="|"<="|">="|"+"|"-"|"*"|"/"|"="             {printf("<OP, %s>\n", yytext);}
[ \t\n]+                                            {}
.                                                   {printf("<INVALID, %s>\n", yytext);}
%%

int yywrap(){
    return 1;
}

int main(){
    printf("Enter the C code here:\n");
    yylex();
    return 0;
}