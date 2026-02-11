/* Write a LEX program to identify and count keywords and identifiers. */

%{
    #include <stdio.h>
    #include <string.h>
    int keywords = 0;
    int identifiers = 0;
    int is_keyword(char* token) {
        char *keywords[] = {"auto", "break", "case", "char", "const", "continue",
                            "default", "do", "double", "else", "enum", "extern",
                            "float", "for", "goto", "if", "int", "long", "register",
                            "return", "short", "signed", "sizeof", "static", "struct",
                            "switch", "typedef", "union", "unsigned", "void", "volatile",
                            "while"};
        int n = sizeof(keywords) / sizeof(char*);
        for(int i = 0; i < n; i++) {
            if(strcmp(token, keywords[i]) == 0) {
                return 1;
            }
        }
        return 0;
    }
%}

identifier [a-zA-Z_][a-zA-Z0-9_]*
whitespace [ \t\n]+

%%
{identifier}    {
    if(is_keyword(yytext)) {
        keywords++;
        printf("KEYWORD: %s.\n", yytext);
    }
    else {
        identifiers++;
        printf("IDENTIFIER: %s.\n", yytext);
    }
}
{whitespace}    {}
.               {}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Write code here:\n");
    yylex();
    printf("The number of keywords is: %d.\n", keywords);
    printf("The number of identifiers is: %d.\n", identifiers);
}