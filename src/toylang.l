%{
/* ─── ToyLang Lexical Analyzer ─── */
#define _POSIX_C_SOURCE 200809L  /* expose fileno() in strict C99 */
#include <stdio.h>
#include <string.h>
#include "token.h"

int error_count = 0;  /* total lexical errors encountered */
int col = 1;          /* current column tracker            */

/* Update column after each match */
#define UPDATE_COL()  col += yyleng
#define RESET_COL()   col = 1
%}

%option noyywrap yylineno noinput nounput

/* ─── Named Regex Macros ─── */
DIGIT     [0-9]
LETTER    [a-zA-Z_]
ALNUM     [a-zA-Z0-9_]
INT_LIT   {DIGIT}+
FLOAT_LIT {DIGIT}+"."{DIGIT}+
IDENT     {LETTER}{ALNUM}*
WS        [ \t\r]+
NEWLINE   \n

%%
 /* ═══════════════════════════════════════════════════════
  *  RULES — ordered by priority (longest match + first rule wins)
  * ═══════════════════════════════════════════════════════ */

 /* ── 1. Keywords (before IDENT) ── */
"if"        { UPDATE_COL(); return TOK_IF;     }
"else"      { UPDATE_COL(); return TOK_ELSE;   }
"while"     { UPDATE_COL(); return TOK_WHILE;  }
"for"       { UPDATE_COL(); return TOK_FOR;    }
"int"       { UPDATE_COL(); return TOK_INT;    }
"float"     { UPDATE_COL(); return TOK_FLOAT;  }
"bool"      { UPDATE_COL(); return TOK_BOOL;   }
"return"    { UPDATE_COL(); return TOK_RETURN; }
"true"      { UPDATE_COL(); return TOK_TRUE;   }
"false"     { UPDATE_COL(); return TOK_FALSE;  }

 /* ── 2. Float literal (before INT to avoid 3.14 → INT+DOT+INT) ── */
{FLOAT_LIT} { UPDATE_COL(); return TOK_FLOAT_LIT; }

 /* ── 3. Integer literal ── */
{INT_LIT}   { UPDATE_COL(); return TOK_INT_LIT; }

 /* ── 4. Identifiers (after all keywords) ── */
{IDENT}     { UPDATE_COL(); return TOK_IDENT; }

 /* ── 5. Two-char relational operators (before single-char) ── */
"=="        { UPDATE_COL(); return TOK_EQ;  }
"!="        { UPDATE_COL(); return TOK_NEQ; }
"<="        { UPDATE_COL(); return TOK_LEQ; }
">="        { UPDATE_COL(); return TOK_GEQ; }

 /* ── 6. Two-char logical operators ── */
"&&"        { UPDATE_COL(); return TOK_AND; }
"||"        { UPDATE_COL(); return TOK_OR;  }

 /* ── 7. Single-char arithmetic operators ── */
"+"         { UPDATE_COL(); return TOK_PLUS;  }
"-"         { UPDATE_COL(); return TOK_MINUS; }
"*"         { UPDATE_COL(); return TOK_STAR;  }
"/"         { UPDATE_COL(); return TOK_SLASH; }
"%"         { UPDATE_COL(); return TOK_MOD;   }

 /* ── 8. Single-char relational / logical (after 2-char versions) ── */
"<"         { UPDATE_COL(); return TOK_LT;  }
">"         { UPDATE_COL(); return TOK_GT;  }
"!"         { UPDATE_COL(); return TOK_NOT; }

 /* ── 9. Assignment (after ==) ── */
"="         { UPDATE_COL(); return TOK_ASSIGN; }

 /* ── 10. Delimiters ── */
"("         { UPDATE_COL(); return TOK_LPAREN; }
")"         { UPDATE_COL(); return TOK_RPAREN; }
"{"         { UPDATE_COL(); return TOK_LBRACE; }
"}"         { UPDATE_COL(); return TOK_RBRACE; }
";"         { UPDATE_COL(); return TOK_SEMI;   }
","         { UPDATE_COL(); return TOK_COMMA;  }

 /* ── 11. Single-line comment — silently consumed ── */
"//"[^\n]*  { UPDATE_COL(); /* no token emitted */ }

 /* ── 12. Multi-line comment — silently consumed ── */
"/*"([^*]|\*+[^*/])*\*+"/"  { 
    /* Count newlines inside the comment to keep yylineno accurate */
    for (int i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            RESET_COL();
        } else {
            col++;
        }
    }
    /* no token emitted */ 
}

 /* ── 13. Whitespace — silently consumed ── */
{WS}        { UPDATE_COL(); /* skip */ }

 /* ── 14. Newline — consumed, reset column ── */
{NEWLINE}   { RESET_COL(); /* yylineno auto-incremented by %option yylineno */ }

 /* ── 15. Unterminated comment at EOF ── */
<<EOF>>     {
    return TOK_EOF;
}

 /* ── 16. Catch-all: any unrecognized character → LEXICAL_ERROR ── */
.           {
    fprintf(stderr,
        "LEXICAL_ERROR [line %d, col %d]: unexpected character '%s'\n",
        yylineno, col, yytext);
    error_count++;
    UPDATE_COL();
    /* consume the bad character and continue scanning */
}

%%

/* ═══════════════════════════════════════════════════════
 *  USER CODE SECTION — helper functions
 * ═══════════════════════════════════════════════════════ */

const char *token_name(int type) {
    switch (type) {
        /* Keywords */
        case TOK_INT:       return "TOK_INT";
        case TOK_FLOAT:     return "TOK_FLOAT";
        case TOK_BOOL:      return "TOK_BOOL";
        case TOK_IF:        return "TOK_IF";
        case TOK_ELSE:      return "TOK_ELSE";
        case TOK_WHILE:     return "TOK_WHILE";
        case TOK_FOR:       return "TOK_FOR";
        case TOK_RETURN:    return "TOK_RETURN";
        case TOK_TRUE:      return "TOK_TRUE";
        case TOK_FALSE:     return "TOK_FALSE";

        /* Identifiers & Literals */
        case TOK_IDENT:     return "TOK_IDENT";
        case TOK_INT_LIT:   return "TOK_INT_LIT";
        case TOK_FLOAT_LIT: return "TOK_FLOAT_LIT";

        /* Arithmetic */
        case TOK_PLUS:      return "TOK_PLUS";
        case TOK_MINUS:     return "TOK_MINUS";
        case TOK_STAR:      return "TOK_STAR";
        case TOK_SLASH:     return "TOK_SLASH";
        case TOK_MOD:       return "TOK_MOD";

        /* Relational */
        case TOK_EQ:        return "TOK_EQ";
        case TOK_NEQ:       return "TOK_NEQ";
        case TOK_LT:        return "TOK_LT";
        case TOK_GT:        return "TOK_GT";
        case TOK_LEQ:       return "TOK_LEQ";
        case TOK_GEQ:       return "TOK_GEQ";

        /* Logical */
        case TOK_AND:       return "TOK_AND";
        case TOK_OR:        return "TOK_OR";
        case TOK_NOT:       return "TOK_NOT";

        /* Assignment & Delimiters */
        case TOK_ASSIGN:    return "TOK_ASSIGN";
        case TOK_LPAREN:    return "TOK_LPAREN";
        case TOK_RPAREN:    return "TOK_RPAREN";
        case TOK_LBRACE:    return "TOK_LBRACE";
        case TOK_RBRACE:    return "TOK_RBRACE";
        case TOK_SEMI:      return "TOK_SEMI";
        case TOK_COMMA:     return "TOK_COMMA";

        /* Special */
        case TOK_EOF:       return "TOK_EOF";
        case TOK_ERROR:     return "TOK_ERROR";

        default:            return "UNKNOWN";
    }
}
