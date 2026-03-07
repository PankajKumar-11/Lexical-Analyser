#ifndef TOKEN_H
#define TOKEN_H

/* ─── TokenType Enum ─── */
typedef enum {
    /* Data-type keywords */
    TOK_INT, TOK_FLOAT, TOK_BOOL,

    /* Control-flow keywords */
    TOK_IF, TOK_ELSE, TOK_WHILE, TOK_FOR, TOK_RETURN,

    /* Boolean literal keywords */
    TOK_TRUE, TOK_FALSE,

    /* Identifiers & Literals */
    TOK_IDENT, TOK_INT_LIT, TOK_FLOAT_LIT,

    /* Arithmetic operators: + - * / % */
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_MOD,

    /* Relational operators: == != < > <= >= */
    TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LEQ, TOK_GEQ,

    /* Logical operators: && || ! */
    TOK_AND, TOK_OR, TOK_NOT,

    /* Assignment & Delimiters */
    TOK_ASSIGN,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_SEMI, TOK_COMMA,

    /* Special */
    TOK_EOF, TOK_ERROR
} TokenType;

/* ─── Token Struct ─── */
typedef struct {
    TokenType  type;        /* token category        */
    char       lexeme[256]; /* matched text           */
    int        line;        /* source line number     */
    int        col;         /* source column number   */
} Token;

/* Returns a human-readable string for a TokenType value */
const char *token_name(int type);

#endif /* TOKEN_H */
