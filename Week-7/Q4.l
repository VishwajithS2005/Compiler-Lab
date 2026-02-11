/* Write a LEX program to count the number of vowels in the given input. */

%{
	#include <stdio.h>
	int vowels = 0;
%}

%%
[aeiouAEIOU]	{vowels++;}
.|\n			{}
%%

int yywrap() {
	return 1;
}

int main(){
	printf("Input here: ");
	yylex();
	printf("The number of vowels are: %d.\n", vowels);
	return 0;
}
