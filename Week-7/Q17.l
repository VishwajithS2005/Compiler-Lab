/* Write a LEX program to implement a symbol table. */

%{
    #include <stdio.h>
    #include <string.h>
    char symtab[100][50];
    int count = 0;
    int exists(char *s){
        for(int i=0;i<count;i++)
            if(strcmp(symtab[i],s)==0) return 1;
        return 0;
    }
%}

%%
int|float|char|double|if|else|while|for|return|void {}

[a-zA-Z_][a-zA-Z0-9_]* {
    if(!exists(yytext)){
        strcpy(symtab[count++], yytext);
        printf("Inserted into Symbol Table: %s\n", yytext);
    }
}

[ \t\n]+ ;
. ;
%%

int yywrap(){
    return 1;
}

int main(){
    printf("Enter the C code here:\n");
    yylex();
    printf("\nSymbol Table:\n");
    for(int i=0;i<count;i++)
        printf("%d. %s\n", i+1, symtab[i]);
    return 0;
}
