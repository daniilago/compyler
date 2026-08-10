#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdlib.h>
#include <stdarg.h>

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
    NODE_ERROR,
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

typedef enum {
    SYM_VAR,
    SYM_FUNC
} SymbolKind;

typedef struct Symbol {
    char       name[256];
    char       data_type[64];
    SymbolKind kind;
    int        n_params;
    int        line;
    int        col;
    int        reg_index;
} Symbol;

typedef struct Scope {
    Symbol        *symbols;
    int            count;
    int            capacity;
    struct Scope  *parent; 
} Scope;

Scope  *scope_new(Scope *parent);
void    scope_free(Scope *scope);
Symbol *scope_lookup(Scope *scope, const char *name); 
Symbol *scope_lookup_local(Scope *scope, const char *name); 
void    scope_add(Scope *scope, Symbol sym);

typedef struct {
    FILE *out;
    int   label_count;
    int   in_main;
    int   reg_count;
    int   had_error;
} CodeGen;

// Parser
typedef struct {
    TokenList *tl;
    int        pos;
    int        had_error;
    Scope     *global_scope; 
    Scope     *current_scope;
    CodeGen    cg;        
} Parser;

void     parser_init(Parser *p, TokenList *tl, const char *output_file);
ASTNode *parser_run(Parser *p);
void     parser_close(Parser *p);
 
void ast_print(ASTNode *node, int depth);
void ast_free(ASTNode *node);

#endif