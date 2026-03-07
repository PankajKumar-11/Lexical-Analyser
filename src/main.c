/* ToyLang Lexical Analyzer - Driver */
#include <stdio.h>
#include "token.h"

/* Flex-generated globals */
extern FILE *yyin;
extern char *yytext;
extern int   yylineno;
extern int   error_count;
int yylex(void);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: toylang_lexer <source-file>\n");
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror(argv[1]);
        return 1;
    }

    printf("======================================================\n");
    printf("        ToyLang Lexical Analyzer - v1.0\n");
    printf("======================================================\n");
    printf("  Source file: %s\n\n", argv[1]);
    printf("  %-8s  %-16s  %s\n", "Line", "Token", "Lexeme");
    printf("  %-8s  %-16s  %s\n", "----", "-------------", "--------------");

    int tok;
    int token_count = 0;

    while ((tok = yylex()) != TOK_EOF) {
        printf("  [Line %2d]  %-16s  '%s'\n",
               yylineno, token_name(tok), yytext);
        token_count++;
    }

    fclose(yyin);

    printf("\n  Summary --------------------------------------------\n");
    printf("  Total tokens : %d\n", token_count);

    if (error_count > 0) {
        printf("  Lexical errors: %d (see stderr)\n", error_count);
    } else {
        printf("  Lexical errors: 0 - clean scan\n");
    }

    printf("  ----------------------------------------------------\n");

    return (error_count > 0) ? 1 : 0;
}
