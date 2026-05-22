#include "parser.h"

static void advance(Parser *p) {
    p->previous = p->current;
    p->current  = next_token(p->lexer);
}

static void parse_error(Parser *p, const char *msg) {
    p->had_error = 1;
    printf("[Syntatic error][l=%d][c=%d] %s, found: '%s'\n",
        p->current.line, p->current.col,
        msg, p->current.value);
}

static int check(Parser *p, TokenType type) {
    return p->current.type == type;
}

static int check_value(Parser *p, const char *val) {
    int i = 0;
    while (val[i] && p->current.value[i])
        if (val[i] != p->current.value[i]) return 0;
        else i++;
    return val[i] == '\0' && p->current.value[i] == '\0';
}

static void expect(Parser *p, TokenType type, const char *msg) {
    if (check(p, type)) advance(p);
    else                parse_error(p, msg);
}

static void expect_value(Parser *p, const char *val, const char *msg) {
    if (check_value(p, val)) advance(p);
    else                     parse_error(p, msg);
}

static void skip_comments(Parser *p) {
    while (p->current.type == TOKEN_COMMENT)
        advance(p);
}

// Functions declaration
static void parse_statement(Parser *p);
static void parse_expression(Parser *p);
static void parse_block(Parser *p);

// Expressions
static void parse_primary(Parser *p) {
    if (check(p, TOKEN_NUMBER)) {
        advance(p); 
        return;
    }
    if (check(p, TOKEN_IDENT)) {
        advance(p); 

        if (check_value(p, "(")) {
            advance(p); 
            if (!check_value(p, ")")) {
                parse_expression(p);
                while (check_value(p, ",")) {
                    advance(p);
                    parse_expression(p);
                }
            }
            expect_value(p, ")", "Expected ')' after arguments");
        }
        return;
    }
    if (check(p, TOKEN_LITERAL_STR) || check(p, TOKEN_LITERAL_CHAR)) {
        advance(p);
        return;
    }

    if (check_value(p, "(")) {
        advance(p);
        parse_expression(p);
        expect_value(p, ")", "Expected ')' closing expression");
        return;
    }

    parse_error(p, "Invalid expression");
    advance(p); 
}

static void parse_expression(Parser *p) {
    parse_primary(p);

    while (check(p, TOKEN_ARITHMETIC_OP) || check(p, TOKEN_LOGIC_OP)) {
        advance(p);          
        parse_primary(p);    
    }
}

static void parse_declaration(Parser *p) {
    advance(p); 

    expect(p, TOKEN_IDENT, "Expected identifier after type");

    if (check_value(p, "=")) {
        advance(p);
        parse_expression(p);
    }

    expect_value(p, ";", "Expected ';' after declaration");
}

static void parse_assignment(Parser *p) {
    advance(p); 
    expect_value(p, "=", "Expected '=' after identifier");
    parse_expression(p);
    expect_value(p, ";", "Expected ';' after assignment");
}

static void parse_if(Parser *p) {
    advance(p); 
    expect_value(p, "(", "Expected '(' after 'if'");
    parse_expression(p);
    expect_value(p, ")", "Expected ')' after condition");
    parse_block(p);

    if (check_value(p, "else")) {
        advance(p);
        parse_block(p);
    }
}

static void parse_while(Parser *p) {
    advance(p); 
    expect_value(p, "(", "Expected '(' after 'while'");
    parse_expression(p);
    expect_value(p, ")", "Expected ')' after condition");
    parse_block(p);
}

static void parse_for(Parser *p) {
    advance(p); 
    expect_value(p, "(", "Expected '(' after 'for'");
    parse_statement(p);      
    parse_expression(p);     
    expect_value(p, ";", "Expected ';' after for condition");
    parse_expression(p);     
    expect_value(p, ")", "Expected ')' closing 'for'");
    parse_block(p);
}

static void parse_return(Parser *p) {
    advance(p); 
    if (!check_value(p, ";"))
        parse_expression(p);
    expect_value(p, ";", "Expected ';' after return");
}


static void parse_block(Parser *p) {
    expect_value(p, "{", "Expected '{'");
    skip_comments(p);
    while (!check_value(p, "}") && !check(p, TOKEN_EOF)) {
        parse_statement(p);
        skip_comments(p);
    }
    expect_value(p, "}", "Expected '}'");
}


static void parse_statement(Parser *p) {
    skip_comments(p);

    if (check(p, TOKEN_RESERVED_WORD) && (
        check_value(p, "int")   ||
        check_value(p, "float") ||
        check_value(p, "char"))) {
        parse_declaration(p);
        return;
    }
    if (check_value(p, "if"))     { parse_if(p);     return; }
    if (check_value(p, "while"))  { parse_while(p);  return; }
    if (check_value(p, "for"))    { parse_for(p);    return; }
    if (check_value(p, "return")) { parse_return(p); return; }

    if (check(p, TOKEN_IDENT)) {
        parse_assignment(p);
        return;
    }

    parse_error(p, "Invalid statement");
    advance(p);
}

static void parse_function(Parser *p) {
    advance(p); 
    expect(p, TOKEN_IDENT, "Expected function name");
    expect_value(p, "(", "Expected '(' after function name");

    if (!check_value(p, ")")) {
        expect(p, TOKEN_RESERVED_WORD, "Expected parameter type");
        expect(p, TOKEN_IDENT, "Expected parameter name");
        while (check_value(p, ",")) {
            advance(p);
            expect(p, TOKEN_RESERVED_WORD, "Expected parameter type");
            expect(p, TOKEN_IDENT, "Expected parameter name");
        }
    }

    expect_value(p, ")", "Expected ')' closing parameters");
    parse_block(p);
}

// Initialize parser
void parser_init(Parser *p, Lexer *l) {
    p->lexer     = l;
    p->had_error = 0;
    advance(p); 
}

void parser_run(Parser *p) {
    skip_comments(p);
    while (!check(p, TOKEN_EOF)) {
        if (check(p, TOKEN_RESERVED_WORD))
            parse_function(p);
        else {
            parse_error(p, "Expected function declaration");
            advance(p);
        }
        skip_comments(p);
    }

    if (!p->had_error)
        printf("Syntactic analysis completed without errors\n");
}