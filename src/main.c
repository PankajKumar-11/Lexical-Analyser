#include <stdio.h>
#include "token.h"
#include "parser.h"

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
    printf("        ToyLang Predictive Parser - v1.0\n");
    printf("======================================================\n");
    printf("  Source file: %s\n\n", argv[1]);

    int parse_errors = parse();

    fclose(yyin);

    printf("\n  Summary --------------------------------------------\n");

    if (error_count > 0 || parse_errors > 0) {
        printf("  Lexical errors: %d\n", error_count);
        printf("  Syntax errors:  %d\n", parse_errors);
    } else {
        printf("  Errors: 0 - smooth parsing!\n");
    }

    printf("  ----------------------------------------------------\n");

    return (error_count > 0 || parse_errors > 0) ? 1 : 0;
}
