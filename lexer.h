#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>

typedef enum {
    TOKEN_NUMBER,
    TOKEN_IDENT,
    TOKEN_RESERVED_WORD,
    TOKEN_LOGIC_OP,
    TOKEN_ARITHMETIC_OP,
    TOKEN_SEPARATOR,
    TOKEN_COMMENT,
    TOKEN_LITERAL_STR,
    TOKEN_LITERAL_CHAR,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
    int line;
    int col;
} Token;

typedef struct {
    const char *src;
    int pos;
    int line;
    int col;
} Lexer;

void lexer_init(Lexer *l, const char *src);
Token next_token(Lexer *l);
void print_token(const Token *tok);

#endif