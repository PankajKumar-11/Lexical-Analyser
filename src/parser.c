#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

/* External functions/variables from Flex Lexer */
extern int yylex(void);
extern char *yytext;
extern int yylineno;

#define MAX_STACK_SIZE 1024

static StackSymbol stack[MAX_STACK_SIZE];
static int top_idx = -1;

void push_nt(NonTerminal nt) {
    if (top_idx >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Parser Error: Stack overflow\n");
        exit(1);
    }
    top_idx++;
    stack[top_idx].type = SYM_NON_TERMINAL;
    stack[top_idx].val.nt = nt;
}

void push_t(TokenType t) {
    if (top_idx >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Parser Error: Stack overflow\n");
        exit(1);
    }
    top_idx++;
    stack[top_idx].type = SYM_TERMINAL;
    stack[top_idx].val.t = t;
}

StackSymbol pop() {
    if (top_idx < 0) {
        fprintf(stderr, "Parser Error: Stack underflow\n");
        exit(1);
    }
    return stack[top_idx--];
}

StackSymbol top_sym() {
    if (top_idx < 0) {
        fprintf(stderr, "Parser Error: Stack is empty\n");
        exit(1);
    }
    return stack[top_idx];
}

const char* nt_name(NonTerminal nt) {
    switch (nt) {
        case NT_Expr: return "Expr";
        case NT_LOr: return "LOr";
        case NT_LOrPrime: return "LOr'";
        case NT_LAnd: return "LAnd";
        case NT_LAndPrime: return "LAnd'";
        case NT_Eq: return "Eq";
        case NT_EqPrime: return "Eq'";
        case NT_Rel: return "Rel";
        case NT_RelPrime: return "Rel'";
        case NT_Add: return "Add";
        case NT_AddPrime: return "Add'";
        case NT_Mult: return "Mult";
        case NT_MultPrime: return "Mult'";
        case NT_Unary: return "Unary";
        case NT_Primary: return "Primary";
        default: return "Unknown";
    }
}

/* Print stack contents for tracing */
void print_stack() {
    printf("Stack: [");
    for (int i = 0; i <= top_idx; ++i) {
        if (stack[i].type == SYM_TERMINAL) {
            printf(" %s", token_name(stack[i].val.t));
        } else {
            printf(" %s", nt_name(stack[i].val.nt));
        }
    }
    printf(" ]\n");
}

/* Synchronizing sets (simplification) */
/* We consider EOF, RPAREN, SEMI as general synchronizing tokens. */
int is_synch(NonTerminal nt, TokenType t) {
    if (t == TOK_EOF || t == TOK_RPAREN || t == TOK_SEMI) return 1;
    return 0;
}

/* Apply production logic M[X, a]. 
   Returns 1 if valid, 0 if syntax error. */
int apply_production(NonTerminal X, TokenType a) {
    /* To push RHS in reverse order */
    switch (X) {
        case NT_Expr:
            /* Expr -> LOr */
            if (a == TOK_NOT || a == TOK_MINUS || a == TOK_LPAREN || a == TOK_IDENT || a == TOK_INT_LIT || a == TOK_FLOAT_LIT || a == TOK_TRUE || a == TOK_FALSE) {
                printf("Action: Expr -> LOr\n");
                push_nt(NT_LOr);
                return 1;
            }
            break;
            
        case NT_LOr:
            /* LOr -> LAnd LOr' */
            if (a == TOK_NOT || a == TOK_MINUS || a == TOK_LPAREN || a == TOK_IDENT || a == TOK_INT_LIT || a == TOK_FLOAT_LIT || a == TOK_TRUE || a == TOK_FALSE) {
                printf("Action: LOr -> LAnd LOr'\n");
                push_nt(NT_LOrPrime);
                push_nt(NT_LAnd);
                return 1;
            }
            break;
            
        case NT_LOrPrime:
            /* LOr' -> '||' LAnd LOr' | epsilon */
            if (a == TOK_OR) {
                printf("Action: LOr' -> || LAnd LOr'\n");
                push_nt(NT_LOrPrime);
                push_nt(NT_LAnd);
                push_t(TOK_OR);
                return 1;
            } else if (a == TOK_EOF || a == TOK_RPAREN || a == TOK_SEMI || a == TOK_COMMA) {
                /* epsilon */
                printf("Action: LOr' -> epsilon\n");
                return 1;
            }
            break;

        case NT_LAnd:
            /* LAnd -> Eq LAnd' */
            if (a == TOK_NOT || a == TOK_MINUS || a == TOK_LPAREN || a == TOK_IDENT || a == TOK_INT_LIT || a == TOK_FLOAT_LIT || a == TOK_TRUE || a == TOK_FALSE) {
                printf("Action: LAnd -> Eq LAnd'\n");
                push_nt(NT_LAndPrime);
                push_nt(NT_Eq);
                return 1;
            }
            break;

        case NT_LAndPrime:
            /* LAnd' -> '&&' Eq LAnd' | epsilon */
            if (a == TOK_AND) {
                printf("Action: LAnd' -> && Eq LAnd'\n");
                push_nt(NT_LAndPrime);
                push_nt(NT_Eq);
                push_t(TOK_AND);
                return 1;
            } else if (a == TOK_OR || a == TOK_EOF || a == TOK_RPAREN || a == TOK_SEMI || a == TOK_COMMA) {
                printf("Action: LAnd' -> epsilon\n");
                return 1;
            }
            break;

        case NT_Eq:
            /* Eq -> Rel Eq' */
            if (a == TOK_NOT || a == TOK_MINUS || a == TOK_LPAREN || a == TOK_IDENT || a == TOK_INT_LIT || a == TOK_FLOAT_LIT || a == TOK_TRUE || a == TOK_FALSE) {
                printf("Action: Eq -> Rel Eq'\n");
                push_nt(NT_EqPrime);
                push_nt(NT_Rel);
                return 1;
            }
            break;

        case NT_EqPrime:
            /* Eq' -> '==' Rel Eq' | '!=' Rel Eq' | epsilon */
            if (a == TOK_EQ) {
                printf("Action: Eq' -> == Rel Eq'\n");
                push_nt(NT_EqPrime);
                push_nt(NT_Rel);
                push_t(TOK_EQ);
                return 1;
            } else if (a == TOK_NEQ) {
                printf("Action: Eq' -> != Rel Eq'\n");
                push_nt(NT_EqPrime);
                push_nt(NT_Rel);
                push_t(TOK_NEQ);
                return 1;
            } else if (a == TOK_AND || a == TOK_OR || a == TOK_EOF || a == TOK_RPAREN || a == TOK_SEMI || a == TOK_COMMA) {
                printf("Action: Eq' -> epsilon\n");
                return 1;
            }
            break;

        case NT_Rel:
            /* Rel -> Add Rel' */
            if (a == TOK_NOT || a == TOK_MINUS || a == TOK_LPAREN || a == TOK_IDENT || a == TOK_INT_LIT || a == TOK_FLOAT_LIT || a == TOK_TRUE || a == TOK_FALSE) {
                printf("Action: Rel -> Add Rel'\n");
                push_nt(NT_RelPrime);
                push_nt(NT_Add);
                return 1;
            }
            break;

        case NT_RelPrime:
            /* Rel' -> '<' Add Rel' | '>' Add Rel' | '<=' Add Rel' | '>=' Add Rel' | epsilon */
            if (a == TOK_LT) {
                printf("Action: Rel' -> < Add Rel'\n");
                push_nt(NT_RelPrime);
                push_nt(NT_Add);
                push_t(TOK_LT);
                return 1;
            } else if (a == TOK_GT) {
                printf("Action: Rel' -> > Add Rel'\n");
                push_nt(NT_RelPrime);
                push_nt(NT_Add);
                push_t(TOK_GT);
                return 1;
            } else if (a == TOK_LEQ) {
                printf("Action: Rel' -> <= Add Rel'\n");
                push_nt(NT_RelPrime);
                push_nt(NT_Add);
                push_t(TOK_LEQ);
                return 1;
            } else if (a == TOK_GEQ) {
                printf("Action: Rel' -> >= Add Rel'\n");
                push_nt(NT_RelPrime);
                push_nt(NT_Add);
                push_t(TOK_GEQ);
                return 1;
            } else if (a == TOK_EQ || a == TOK_NEQ || a == TOK_AND || a == TOK_OR || a == TOK_EOF || a == TOK_RPAREN || a == TOK_SEMI || a == TOK_COMMA) {
                printf("Action: Rel' -> epsilon\n");
                return 1;
            }
            break;

        case NT_Add:
            /* Add -> Mult Add' */
            if (a == TOK_NOT || a == TOK_MINUS || a == TOK_LPAREN || a == TOK_IDENT || a == TOK_INT_LIT || a == TOK_FLOAT_LIT || a == TOK_TRUE || a == TOK_FALSE) {
                printf("Action: Add -> Mult Add'\n");
                push_nt(NT_AddPrime);
                push_nt(NT_Mult);
                return 1;
            }
            break;

        case NT_AddPrime:
            /* Add' -> '+' Mult Add' | '-' Mult Add' | epsilon */
            if (a == TOK_PLUS) {
                printf("Action: Add' -> + Mult Add'\n");
                push_nt(NT_AddPrime);
                push_nt(NT_Mult);
                push_t(TOK_PLUS);
                return 1;
            } else if (a == TOK_MINUS) {
                printf("Action: Add' -> - Mult Add'\n");
                push_nt(NT_AddPrime);
                push_nt(NT_Mult);
                push_t(TOK_MINUS);
                return 1;
            } else if (a == TOK_LT || a == TOK_GT || a == TOK_LEQ || a == TOK_GEQ || a == TOK_EQ || a == TOK_NEQ || a == TOK_AND || a == TOK_OR || a == TOK_EOF || a == TOK_RPAREN || a == TOK_SEMI || a == TOK_COMMA) {
                printf("Action: Add' -> epsilon\n");
                return 1;
            }
            break;

        case NT_Mult:
            /* Mult -> Unary Mult' */
            if (a == TOK_NOT || a == TOK_MINUS || a == TOK_LPAREN || a == TOK_IDENT || a == TOK_INT_LIT || a == TOK_FLOAT_LIT || a == TOK_TRUE || a == TOK_FALSE) {
                printf("Action: Mult -> Unary Mult'\n");
                push_nt(NT_MultPrime);
                push_nt(NT_Unary);
                return 1;
            }
            break;

        case NT_MultPrime:
            /* Mult' -> '*' Unary Mult' | '/' Unary Mult' | '%' Unary Mult' | epsilon */
            if (a == TOK_STAR) {
                printf("Action: Mult' -> * Unary Mult'\n");
                push_nt(NT_MultPrime);
                push_nt(NT_Unary);
                push_t(TOK_STAR);
                return 1;
            } else if (a == TOK_SLASH) {
                printf("Action: Mult' -> / Unary Mult'\n");
                push_nt(NT_MultPrime);
                push_nt(NT_Unary);
                push_t(TOK_SLASH);
                return 1;
            } else if (a == TOK_MOD) {
                printf("Action: Mult' -> %% Unary Mult'\n");
                push_nt(NT_MultPrime);
                push_nt(NT_Unary);
                push_t(TOK_MOD);
                return 1;
            } else if (a == TOK_PLUS || a == TOK_MINUS || a == TOK_LT || a == TOK_GT || a == TOK_LEQ || a == TOK_GEQ || a == TOK_EQ || a == TOK_NEQ || a == TOK_AND || a == TOK_OR || a == TOK_EOF || a == TOK_RPAREN || a == TOK_SEMI || a == TOK_COMMA) {
                printf("Action: Mult' -> epsilon\n");
                return 1;
            }
            break;

        case NT_Unary:
            /* Unary -> '!' Unary | '-' Unary | Primary */
            if (a == TOK_NOT) {
                printf("Action: Unary -> ! Unary\n");
                push_nt(NT_Unary);
                push_t(TOK_NOT);
                return 1;
            } else if (a == TOK_MINUS) {
                printf("Action: Unary -> - Unary\n");
                push_nt(NT_Unary);
                push_t(TOK_MINUS);
                return 1;
            } else if (a == TOK_LPAREN || a == TOK_IDENT || a == TOK_INT_LIT || a == TOK_FLOAT_LIT || a == TOK_TRUE || a == TOK_FALSE) {
                printf("Action: Unary -> Primary\n");
                push_nt(NT_Primary);
                return 1;
            }
            break;

        case NT_Primary:
            /* Primary -> '(' Expr ')' | id | int_lit | float_lit | true | false */
            if (a == TOK_LPAREN) {
                printf("Action: Primary -> ( Expr )\n");
                push_t(TOK_RPAREN);
                push_nt(NT_Expr);
                push_t(TOK_LPAREN);
                return 1;
            } else if (a == TOK_IDENT) {
                printf("Action: Primary -> id (%s)\n", yytext);
                push_t(TOK_IDENT);
                return 1;
            } else if (a == TOK_INT_LIT) {
                printf("Action: Primary -> int_lit (%s)\n", yytext);
                push_t(TOK_INT_LIT);
                return 1;
            } else if (a == TOK_FLOAT_LIT) {
                printf("Action: Primary -> float_lit (%s)\n", yytext);
                push_t(TOK_FLOAT_LIT);
                return 1;
            } else if (a == TOK_TRUE) {
                printf("Action: Primary -> true\n");
                push_t(TOK_TRUE);
                return 1;
            } else if (a == TOK_FALSE) {
                printf("Action: Primary -> false\n");
                push_t(TOK_FALSE);
                return 1;
            }
            break;
    }

    return 0; // Syntax error
}

int parse(void) {
    int error_count = 0;
    TokenType a = yylex();

    // Initialize stack
    top_idx = -1;
    push_t(TOK_EOF);
    push_nt(NT_Expr);

    while (top_idx >= 0) {
        print_stack();
        StackSymbol X = top_sym();
        printf("Input: %-15s ('%s')\n", token_name(a), yytext);

        if (X.type == SYM_TERMINAL) {
            if (X.val.t == a) {
                pop();
                if (a != TOK_EOF) {
                    printf("Action: Matched %s\n", token_name(a));
                    a = yylex();
                } else {
                    printf("Action: Matched TOK_EOF. Parsing complete.\n");
                }
            } else {
                /* Error: Expected terminal, got something else */
                error_count++;
                fprintf(stderr, "Syntax Error [line %d]: Expected %s, found %s ('%s')\n",
                        yylineno, token_name(X.val.t), token_name(a), yytext);
                /* Panic mode: pop the terminal and pretend it was there */
                pop();
                printf("Action: Error Recovery - popped expected terminal %s from stack.\n", token_name(X.val.t));
            }
        } else {
            /* Non-terminal */
            pop();
            if (!apply_production(X.val.nt, a)) {
                error_count++;
                fprintf(stderr, "Syntax Error [line %d]: Unexpected token %s ('%s') for %s\n",
                        yylineno, token_name(a), yytext, nt_name(X.val.nt));
                        
                /* Panic Mode Recovery */
                if (is_synch(X.val.nt, a)) {
                    printf("Action: Error Recovery - popped non-terminal %s (synch found: %s)\n", nt_name(X.val.nt), token_name(a));
                    /* Non-terminal is already popped, continue analyzing with current input token */
                } else {
                    printf("Action: Error Recovery - skipping token %s\n", token_name(a));
                    push_nt(X.val.nt); /* Re-push the non-terminal since we're skipping the input token */
                    a = yylex();
                }
            }
        }
        printf("--------------------------------------------------------------------------------\n");
    }
    
    return error_count;
}
