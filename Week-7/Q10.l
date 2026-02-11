/* Write a LEX program to remove single-line and multi-line comments from a C program. */

%{
    #include <stdio.h>
    #include <string.h>
    #define MAX 100000
    char code[MAX];
    int x = 0;
    void add_text(const char *text) {
        int len = strlen(text);
        if (x + len < MAX) {
            strcpy(&code[x], text);
            x += len;
        }
    }
%}

%x SINGLE MULTI

%%
\"([^\\\"]|\\.)*\"  {add_text(yytext);}
\'([^\\\']|\\.)*\'  {add_text(yytext);}
"//"    {BEGIN(SINGLE);}
"/*"    {BEGIN(MULTI);}

<SINGLE>\n  {BEGIN(INITIAL); add_text("\n");}
<SINGLE>.   {}

<MULTI>"*/" {BEGIN(INITIAL);}
<MULTI>\n   {add_text("\n");}
<MULTI>.    {}

.|\n    {add_text(yytext);}
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Enter C Code:\n");
    yylex();
    printf("The C code without any comments is:\n\n%s\n", code);
    return 0;
}