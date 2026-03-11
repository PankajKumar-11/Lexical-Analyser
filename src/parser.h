#ifndef PARSER_H
#define PARSER_H

#include "token.h"

/* Non-terminals in the LL(1) Expression Grammar */
typedef enum {
    NT_Expr,
    NT_LOr,
    NT_LOrPrime,
    NT_LAnd,
    NT_LAndPrime,
    NT_Eq,
    NT_EqPrime,
    NT_Rel,
    NT_RelPrime,
    NT_Add,
    NT_AddPrime,
    NT_Mult,
    NT_MultPrime,
    NT_Unary,
    NT_Primary
} NonTerminal;

/* Stack symbol type */
typedef enum {
    SYM_TERMINAL,
    SYM_NON_TERMINAL
} SymbolType;

typedef struct {
    SymbolType type;
    union {
        TokenType t;
        NonTerminal nt;
    } val;
} StackSymbol;

/* Returns a human-readable string for a NonTerminal value */
const char* nt_name(NonTerminal nt);

/* Main parser function. Returns 0 on success, >0 on errors. */
int parse(void);

#endif /* PARSER_H */
