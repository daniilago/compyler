#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdlib.h>

// AST node types
typedef enum {
    NODE_PROGRAM,       
    NODE_FUNCTION,      
    NODE_PARAM,         
    NODE_BLOCK,         
    NODE_DECL,          
    NODE_ASSIGN,        
    NODE_IF,            
    NODE_WHILE,         
    NODE_FOR,           
    NODE_RETURN,        
    NODE_EXPR_STMT,     
    NODE_BINOP,         
    NODE_CALL,      
    NODE_NUMBER,       
    NODE_IDENT,        
    NODE_LITERAL_STR,  
    NODE_LITERAL_CHAR,  
} NodeType;

// AST
typedef struct ASTNode {
    NodeType        type;
    char            value[256];
    char            data_type[64];
    int             line;
    int             col;
    struct ASTNode **children;
    int              n_children;
} ASTNode;

// Parser
typedef struct {
    TokenList *tl;        
    int        pos;      
    int        had_error; 
} Parser;

void parser_init(Parser *p, TokenList *tl);
ASTNode *parser_run(Parser *p);
 
void ast_print(ASTNode *node, int depth);
void ast_free(ASTNode *node);


#endif