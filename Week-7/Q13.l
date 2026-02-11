/* Write a LEX program to identify valid and invalid identifiers. */

%{
    #include <stdio.h>
    #include <string.h>
    const char *keywords[] = {
        "int","float","char","double","if","else","while","for",
        "return","void","break","continue","switch","case","default",
        "do","struct","union","typedef","enum","const","unsigned",
        "signed","extern","static","sizeof","long","short","goto"
    };
    int isKeyword(const char *word) {
        int n = sizeof(keywords)/sizeof(keywords[0]);
        for(int i = 0; i < n; i++) {
            if(strcmp(word, keywords[i]) == 0)
                return 1;
        }
        return 0;
    }
%}

valid_id    [a-zA-Z_][a-zA-Z0-9_]*
invalid_id  [0-9][a-zA-Z0-9_]*

%%
{valid_id} {
    if(isKeyword(yytext))
        printf("%s : Keyword (not identifier)\n", yytext);
    else
        printf("%s : Valid Identifier\n", yytext);
}

{invalid_id}    {printf("%s : Invalid Identifier\n", yytext);}

[ \t\n]+        {}

.               {printf("%s : Invalid Identifier\n", yytext);}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Enter tokens:\n");
    yylex();
    return 0;
}
