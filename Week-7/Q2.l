/* Write a LEX program to count the number of words in a sentence. */

%{
    #include <stdio.h>
    int words = 0;
%}

%%
[a-zA-z]+   {words++;}
[ \n\t]+    {}
.           {}
%%

int yywrap(){
    return 1;
}

int main(){
    printf("Input here: ");
    yylex();
    printf("The number of words is: %d.\n", words);
    return 0;
}