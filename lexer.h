#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//  Tipos de token  

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
    TOKEN_ERROR,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
    int line;
    int col;
} Token;

//  Lista de tokens 

typedef struct {
    Token *tokens;
    int    count;
    int    capacity;
} TokenList;
 
void  token_list_init(TokenList *tl);
void  token_list_push(TokenList *tl, Token tok);
void  token_list_free(TokenList *tl);
void  token_list_print(const TokenList *tl);

//  Lexer  

typedef struct {
    const char *src;
    int pos;
    int line;
    int col;
} Lexer;

void lexer_init(Lexer *l, const char *src);
Token next_token(Lexer *l);
TokenList lexer_tokenize(Lexer *l);
void print_token(const Token *tok);

#endif