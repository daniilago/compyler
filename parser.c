#include "parser.h"

// New node
static ASTNode *node_new(NodeType type, const char *value, int line, int col) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = type;
    n->line = line;
    n->col = col;
    n->children = NULL;
    n->n_children = 0;
    n->data_type[0] = '\0';
    strncpy(n->value, value ? value : "", 255);
    n->value[255] = '\0';
    return n;
}

// Add child to node
static void node_add_child(ASTNode *parent, ASTNode *child) {
    parent->n_children++;
    parent->children = realloc(parent->children, parent->n_children * sizeof(ASTNode *));
    parent->children[parent->n_children - 1] = child;
}

// Print
static const char *node_type_str(NodeType t) {
    switch (t) {
        case NODE_PROGRAM:      return "PROGRAM";
        case NODE_FUNCTION:     return "FUNCTION";
        case NODE_PARAM:        return "PARAM";
        case NODE_BLOCK:        return "BLOCK";
        case NODE_DECL:         return "DECL";
        case NODE_ASSIGN:       return "ASSIGN";
        case NODE_IF:           return "IF";
        case NODE_WHILE:        return "WHILE";
        case NODE_FOR:          return "FOR";
        case NODE_RETURN:       return "RETURN";
        case NODE_EXPR_STMT:    return "EXPR_STMT";
        case NODE_BINOP:        return "BINOP";
        case NODE_CALL:         return "CALL";
        case NODE_NUMBER:       return "NUMBER";
        case NODE_IDENT:        return "IDENT";
        case NODE_LITERAL_STR:  return "LITERAL_STR";
        case NODE_LITERAL_CHAR: return "LITERAL_CHAR";
        default:                return "?";
    }
}

void ast_print(ASTNode *node, int depth) {
    int i;
    if (!node) return;

    for (i = 0; i < depth; i++) printf("  ");
    printf("%s", node_type_str(node->type));

    if (node->data_type[0])
        printf(" %s %s", node->data_type, node->value);
    else
        printf(" %s", node->value);

    printf("  [l=%d c=%d]\n", node->line, node->col);
    for (i = 0; i < node->n_children; i++)
        ast_print(node->children[i], depth + 1);
}

// Free AST
void ast_free(ASTNode *node) {
    int i;
    if (!node) return;
    
    for (i = 0; i < node->n_children; i++)
        ast_free(node->children[i]);
    free(node->children);
    free(node);
}

// Token utilities
static Token *current(Parser *p) {
    return &p->tl->tokens[p->pos];
}

static void advance(Parser *p) {
    p->pos++;
    while (p->pos < p->tl->count && p->tl->tokens[p->pos].type == TOKEN_COMMENT)
        p->pos++;
}

static int check(Parser *p, TokenType type) {
    return current(p)->type == type;
}

static int check_value(Parser *p, const char *val) {
    return strcmp(current(p)->value, val) == 0;
}

static int expect(Parser *p, TokenType type, const char *msg) {
    if (check(p, type)) { advance(p); return 1; }
    p->had_error = 1;
    printf("[Syntatic error][l=%d][c=%d] %s, found: '%s'\n",
           current(p)->line, current(p)->col, msg, current(p)->value);
    return 0;
}

static int expect_value(Parser *p, const char *val, const char *msg) {
    if (check_value(p, val)) { advance(p); return 1; }
    p->had_error = 1;
    printf("[Syntatic error][l=%d][c=%d] %s, found: '%s'\n",
           current(p)->line, current(p)->col, msg, current(p)->value);
    return 0;
}

static void synchronize(Parser *p) {
    while (!check(p, TOKEN_EOF)) {
        if (check_value(p, ";")) { advance(p); return; }
        if (check_value(p, "}")) { return; }
        advance(p);
    }
}

// Forward declarations
static ASTNode *parse_statement(Parser *p);
static ASTNode *parse_expression(Parser *p);
static ASTNode *parse_block(Parser *p);

// Expressions
static ASTNode *parse_primary(Parser *p) {
    Token *tok = current(p);

    // number
    if (check(p, TOKEN_NUMBER)) {
        ASTNode *n = node_new(NODE_NUMBER, tok->value, tok->line, tok->col);
        advance(p);
        return n;
    }

    // ident or reserved word (function call)
    if (check(p, TOKEN_IDENT)) {
        ASTNode *n = node_new(NODE_IDENT, tok->value, tok->line, tok->col);
        advance(p);

        if (check_value(p, "(")) {
            ASTNode *call = node_new(NODE_CALL, n->value, n->line, n->col);
            ast_free(n); 
            advance(p); 
            if (!check_value(p, ")")) {
                node_add_child(call, parse_expression(p));
                while (check_value(p, ",")) {
                    advance(p);
                    node_add_child(call, parse_expression(p));
                }
            }
            expect_value(p, ")", "Expected ')' after arguments");
            return call;
        }
        return n;
    }

    // string
    if (check(p, TOKEN_LITERAL_STR)) {
        ASTNode *n = node_new(NODE_LITERAL_STR, tok->value, tok->line, tok->col);
        advance(p);
        return n;
    }

    // char
    if (check(p, TOKEN_LITERAL_CHAR)) {
        ASTNode *n = node_new(NODE_LITERAL_CHAR, tok->value, tok->line, tok->col);
        advance(p);
        return n;
    }

    // parenteses
    if (check_value(p, "(")) {
        advance(p);
        ASTNode *n = parse_expression(p);
        expect_value(p, ")", "Expected ')' closing expression");
        return n;
    }

    // error: unexpected token
    p->had_error = 1;
    printf("[Syntatic error][l=%d][c=%d] Invalid expression, found: '%s'\n",
        tok->line, tok->col, tok->value);
    advance(p);
    return node_new(NODE_NUMBER, "?", tok->line, tok->col);
}

static ASTNode *parse_expression(Parser *p) {
    ASTNode *left = parse_primary(p);

    while (check(p, TOKEN_ARITHMETIC_OP) || check(p, TOKEN_LOGIC_OP)) {
        Token *op  = current(p);
        ASTNode *n = node_new(NODE_BINOP, op->value, op->line, op->col);
        advance(p);
        ASTNode *right = parse_primary(p);
        node_add_child(n, left);
        node_add_child(n, right);
        left = n; 
    }

    return left;
}

//Statements
static ASTNode *parse_declaration(Parser *p) {
    Token *type_tok = current(p);
    ASTNode *n = node_new(NODE_DECL, "", type_tok->line, type_tok->col);
    
    // saves the type
    strncpy(n->data_type, type_tok->value, 63);
    advance(p);

    if (check(p, TOKEN_IDENT)) {
        strncpy(n->value, current(p)->value, 255);
        advance(p);
    } else {
        expect(p, TOKEN_IDENT, "Expected identifier after type");
    }

    if (check_value(p, "=")) {
        advance(p);
        node_add_child(n, parse_expression(p));
    }

    expect_value(p, ";", "Expected ';' after declaration");
    return n;
}

static ASTNode *parse_assignment(Parser *p) {
    Token *tok = current(p);
    ASTNode *n = node_new(NODE_ASSIGN, tok->value, tok->line, tok->col);
    advance(p); 
    expect_value(p, "=", "Expected '=' after identifier");
    node_add_child(n, parse_expression(p));
    expect_value(p, ";", "Expected ';' after assignment");
    return n;
}

static ASTNode *parse_if(Parser *p) {
    Token *tok = current(p);
    ASTNode *n = node_new(NODE_IF, "", tok->line, tok->col);
    advance(p); 
    expect_value(p, "(", "Expected '(' after 'if'");
    node_add_child(n, parse_expression(p));
    expect_value(p, ")", "Expected ')' after condition");
    node_add_child(n, parse_block(p));
    if (check_value(p, "else")) {
        advance(p);
        node_add_child(n, parse_block(p));
    }
    return n;
}

static ASTNode *parse_while(Parser *p) {
    Token *tok = current(p);
    ASTNode *n = node_new(NODE_WHILE, "", tok->line, tok->col);
    advance(p);
    expect_value(p, "(", "Expected '(' after 'while'");
    node_add_child(n, parse_expression(p));
    expect_value(p, ")", "Expected ')' after condition");
    node_add_child(n, parse_block(p));
    return n;
}

static ASTNode *parse_return(Parser *p) {
    Token *tok = current(p);
    ASTNode *n = node_new(NODE_RETURN, "", tok->line, tok->col);
    advance(p);
    if (!check_value(p, ";"))
        node_add_child(n, parse_expression(p));
    expect_value(p, ";", "Expected ';' after return");
    return n;
}

static ASTNode *parse_block(Parser *p) {
    Token *tok = current(p);
    ASTNode *n = node_new(NODE_BLOCK, "", tok->line, tok->col);
    expect_value(p, "{", "Expected '{'");
    while (!check_value(p, "}") && !check(p, TOKEN_EOF)) {
        ASTNode *s = parse_statement(p);
        if (s) node_add_child(n, s);
    }
    expect_value(p, "}", "Expected '}'");
    return n;
}

// Decides which statement to parse based on the current token
static ASTNode *parse_statement(Parser *p) {
    if (check(p, TOKEN_RESERVED_WORD) && (
        check_value(p, "int")   ||
        check_value(p, "float") ||
        check_value(p, "char"))) {
        return parse_declaration(p);
    }
    if (check_value(p, "if"))     return parse_if(p);
    if (check_value(p, "while"))  return parse_while(p);
    if (check_value(p, "return")) return parse_return(p);

    if (check(p, TOKEN_IDENT)) {
        int saved = p->pos;
        advance(p);
        int is_call = check_value(p, "(");
        p->pos = saved;

        if (is_call) {
            ASTNode *n = node_new(NODE_EXPR_STMT, "", current(p)->line, current(p)->col);
            node_add_child(n, parse_primary(p));
            expect_value(p, ";", "Expected ';' after function call");
            return n;
        }
        return parse_assignment(p);
    }

    // Check for lexical errors
    if (check(p, TOKEN_ERROR)) {
        p->had_error = 1;
        printf("[Lexical error][l=%d][c=%d] %s\n",
            current(p)->line, current(p)->col, current(p)->value);
        synchronize(p);
        return NULL;
    }

    // error: unexpected token
    p->had_error = 1;
    printf("[Syntatic error][l=%d][c=%d] Invalid statement, found: '%s'\n",
        current(p)->line, current(p)->col, current(p)->value);
    synchronize(p);
    return NULL;
}

// Function
static ASTNode *parse_function(Parser *p) {
    Token *tok = current(p);
    ASTNode *n = node_new(NODE_FUNCTION, "", tok->line, tok->col);
    
    strncpy(n->data_type, tok->value, 63);
    advance(p);

    if (check(p, TOKEN_IDENT)) {
        strncpy(n->value, current(p)->value, 255);
        advance(p);
    } else {
        expect(p, TOKEN_IDENT, "Expected function name");
    }

    expect_value(p, "(", "Expected '(' after function name");

    // parameters
    if (!check_value(p, ")")) {
        Token *pt = current(p);
        ASTNode *param = node_new(NODE_PARAM, "", pt->line, pt->col);

        /* verifica se tem tipo antes de consumir */
        if (check(p, TOKEN_RESERVED_WORD)) {
            strncpy(param->data_type, current(p)->value, 63);
            advance(p);
        } else {
            p->had_error = 1;
            printf("[Syntatic error][l=%d][c=%d] Expected parameter type, found: '%s'\n",
                current(p)->line, current(p)->col, current(p)->value);
        }
        if (check(p, TOKEN_IDENT)) {
            strncpy(param->value, current(p)->value, 255);
            advance(p);
        }
        node_add_child(n, param);

        while (check_value(p, ",")) {
            advance(p);
            pt = current(p);
            param = node_new(NODE_PARAM, "", pt->line, pt->col);

            if (check(p, TOKEN_RESERVED_WORD)) {
                strncpy(param->data_type, current(p)->value, 63);
                advance(p);
            } else {
                p->had_error = 1;
                printf("[Syntatic error][l=%d][c=%d] Expected parameter type, found: '%s'\n",
                    current(p)->line, current(p)->col, current(p)->value);
            }
            if (check(p, TOKEN_IDENT)) {
                strncpy(param->value, current(p)->value, 255);
                advance(p);
            }
            node_add_child(n, param);
        }
    }

    expect_value(p, ")", "Expected ')' closing parameters");
    node_add_child(n, parse_block(p));
    return n;
}

// Init
void parser_init(Parser *p, TokenList *tl) {
    p->tl        = tl;
    p->pos       = 0;
    p->had_error = 0;

    while (p->pos < tl->count &&
           tl->tokens[p->pos].type == TOKEN_COMMENT)
        p->pos++;
}

ASTNode *parser_run(Parser *p) {
    ASTNode *program = node_new(NODE_PROGRAM, "", 0, 0);

    while (!check(p, TOKEN_EOF)) {
        if (!check(p, TOKEN_RESERVED_WORD)) {
            p->had_error = 1;
            printf("[Syntatic error][l=%d][c=%d] Expected declaration, found: '%s'\n",
                current(p)->line, current(p)->col, current(p)->value);
            synchronize(p);
            continue;
        }

        int saved_pos = p->pos;
        advance(p); 
        advance(p); 

        int is_func = check_value(p, "(");
        p->pos = saved_pos;

        if (is_func)
            node_add_child(program, parse_function(p));
        else
            node_add_child(program, parse_declaration(p));
    }

    if (!p->had_error)
        printf("Syntactic analysis completed without errors\n");

    return program;
}