/* Write a LEX program to count the number of characters in a given input. */

%{
#include <stdio.h>
int characters = 0;
%}

%%
.   {characters++;}
\n  {characters++;}
%%

int yywrap(){
    return 1;
}

int main(){
    printf("Input here: ");
    yylex();
    printf("The number of characters in the given input is: %d.\n", characters);
    return 0;
}