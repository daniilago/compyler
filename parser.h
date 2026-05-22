#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct {
    Lexer  *lexer;     
    Token   current;    
    Token   previous;   
    int     had_error;  
} Parser;

void parser_init(Parser *p, Lexer *l);
void parser_run(Parser *p);

#endif