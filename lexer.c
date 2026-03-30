#include "lexer.h"

static const char *RESERVED[] = {
    "if", "else", "while", "for", "return",
    "int", "float", "char", "void", "do",
    NULL
};

static int is_reserved(const char *word) {
    int i;
    for (i = 0; RESERVED[i] != NULL; i++)
        if (strcmp(word, RESERVED[i]) == 0) return 1;
    return 0;
}

void lexer_init(Lexer *l, const char *src) {
    l->src = src;
    l->pos = 0;
    l->line = 1;
    l->col = 1;
}

static char peek(Lexer *l)      { return l->src[l->pos]; }
static char peek2(Lexer *l)     { return l->src[l->pos] ? l->src[l->pos + 1] : '\0'; }
static char advance(Lexer *l) { 
    char c = l->src[l->pos++];
    if (c == '\n') { 
        l->line++; l->col = 1; 
    } else { 
        l->col++;
    }
    return c; 
}
static int  is_digit(char c)    { return c >= '0' && c <= '9'; }
static int  is_alpha(char c)    { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
static int  is_alnum(char c)    { return is_digit(c) || is_alpha(c); }

static void skip_whitespace(Lexer *l) {
    while (peek(l) == ' ' || peek(l) == '\t' || peek(l) == '\n') {
        advance(l);
    }
}

static const char *token_type_str(TokenType t) {
    switch (t) {
        case TOKEN_NUMBER:        return "NUMBER";
        case TOKEN_IDENT:         return "IDENTIFIER";
        case TOKEN_RESERVED_WORD: return "RESERVED_WORD";
        case TOKEN_ARITHMETIC_OP: return "ARITHMETIC_OP";
        case TOKEN_LOGIC_OP:      return "LOGIC_OP";
        case TOKEN_SEPARATOR:     return "SEPARATOR";
        case TOKEN_COMMENT:       return "COMMENT";       
        case TOKEN_LITERAL_STR:   return "LITERAL_STR";   
        case TOKEN_LITERAL_CHAR:  return "LITERAL_CHAR";  
        case TOKEN_EOF:           return "EOF";
        case TOKEN_ERROR:         return "ERROR";
        default:                  return "UNKNOWN";
    }
}

void print_token(const Token *tok) {
    printf("Line: %3d | Col: %3d | Type: %-15s | Value: '%s'\n", 
           tok->line, 
           tok->col, 
           token_type_str(tok->type), 
           tok->value);
}

Token next_token(Lexer *l) {
    Token tok;
    skip_whitespace(l);
    
    tok.line = l->line;
    tok.col = l->col;
    char c = peek(l);

    if (c == '\0') {
        tok.type = TOKEN_EOF;
        tok.value[0] = '\0';
        return tok;
    }

    // Line comment
    if (c == '/' && peek2(l) == '/') {
        int i = 0;
        while (peek(l) != '\n' && peek(l) != '\0')
            tok.value[i++] = advance(l);
        tok.value[i] = '\0';
        tok.type = TOKEN_COMMENT;
        return tok;
    }

    // Block comment
    if (c == '/' && peek2(l) == '*') {
        int i = 0;
        while (!(peek(l) == '*' && peek2(l) == '/') && peek(l) != '\0') {
            tok.value[i++] = advance(l);
        }
        if (peek(l) == '\0') {
            tok.type = TOKEN_ERROR;
            strcpy(tok.value, "Error: Block comment not closed");
            return tok;
        }
        tok.value[i++] = advance(l); 
        tok.value[i++] = advance(l); 
        tok.value[i] = '\0';
        tok.type = TOKEN_COMMENT;
        return tok;
    }

    // Number
    if (is_digit(c)) {
        int start = l->pos;
        while (is_digit(peek(l))) advance(l);

        if (peek(l) == '.') {
            advance(l); 

            if (is_alpha(peek(l)) || peek(l) == ' ' || peek(l) == '\t' || peek(l) == '\n' || peek(l) == '\0') {
                while (peek(l) != ' ' && peek(l) != '\t' && peek(l) != '\n' && peek(l) != '\0') {
                    advance(l);
                }

                tok.type = TOKEN_ERROR;
                strcpy(tok.value, "Error: Invalid number format");
                return tok;
            }
            
            while (is_digit(peek(l))) advance(l);

            if (is_alpha(peek(l)) && peek(l) != ' ' && peek(l) != '\t' && peek(l) != '\n' && peek(l) != '\0') {
                while (peek(l) != ' ' && peek(l) != '\t' && peek(l) != '\n' && peek(l) != '\0') {
                    advance(l);
                }

                tok.type = TOKEN_ERROR;
                strcpy(tok.value, "Error: Invalid number format");
                return tok;
            }
        } else if (is_alpha(peek(l)) && peek(l) != ' ' && peek(l) != '\t' && peek(l) != '\n' && peek(l) != '\0') {
            while (peek(l) != ' ' && peek(l) != '\t' && peek(l) != '\n' && peek(l) != '\0') {
                advance(l);
            }

            tok.type = TOKEN_ERROR;
            strcpy(tok.value, "Error: Invalid number format");
            return tok;
        }

        strncpy(tok.value, l->src + start, l->pos - start);
        tok.value[l->pos - start] = '\0';
        tok.type = TOKEN_NUMBER;
        return tok;
    }

    // Identifier ou reserved_word
    if (is_alpha(c)) {
        int start = l->pos;
        while (is_alnum(peek(l))) advance(l);
        strncpy(tok.value, l->src + start, l->pos - start);
        tok.value[l->pos - start] = '\0';
        tok.type = is_reserved(tok.value) ? TOKEN_RESERVED_WORD : TOKEN_IDENT;
        return tok;
    }

    // Literal string
    if (c == '"') {
        int i = 0;
        tok.value[i++] = advance(l); 
        
        while (peek(l) != '"' && peek(l) != '\0' && peek(l) != '\n') {
            tok.value[i++] = advance(l);
        }
        
        if (peek(l) != '"') {
            tok.type = TOKEN_ERROR;
            strcpy(tok.value, "Error: String not closed");
            return tok;
        }

        tok.value[i++] = advance(l); 
        tok.value[i] = '\0';
        tok.type = TOKEN_LITERAL_STR;
        return tok;
    }

    // Literal char
    if (c == '\'') {
        int i = 0;
        tok.value[i++] = advance(l); 
        while (peek(l) != '\'' && peek(l) != '\0' && peek(l) != '\n') {
            tok.value[i++] = advance(l);
        }

        if (peek(l) != '\'') {
            tok.type = TOKEN_ERROR;
            strcpy(tok.value, "Error: Char not closed");
            return tok;
        }

        tok.value[i++] = advance(l);
        tok.value[i] = '\0';
        tok.type = TOKEN_LITERAL_CHAR;
        return tok;
    }

    // Operators and separators
    tok.col = l->col;
    advance(l);
    tok.value[0] = c;
    tok.value[1] = '\0';

    // Operators with two caracteres
    char next = peek(l);

    if (c == '=' && next == '=') { advance(l); strncpy(tok.value, "==", 2); tok.value[2] = '\0'; tok.type = TOKEN_LOGIC_OP; return tok; }
    if (c == '!' && next == '=') { advance(l); strncpy(tok.value, "!=", 2); tok.value[2] = '\0'; tok.type = TOKEN_LOGIC_OP; return tok; }
    if (c == '<' && next == '=') { advance(l); strncpy(tok.value, "<=", 2); tok.value[2] = '\0'; tok.type = TOKEN_LOGIC_OP; return tok; }
    if (c == '>' && next == '=') { advance(l); strncpy(tok.value, ">=", 2); tok.value[2] = '\0'; tok.type = TOKEN_LOGIC_OP; return tok; }
    if (c == '&' && next == '&') { advance(l); strncpy(tok.value, "&&", 2); tok.value[2] = '\0'; tok.type = TOKEN_LOGIC_OP; return tok; }
    if (c == '|' && next == '|') { advance(l); strncpy(tok.value, "||", 2); tok.value[2] = '\0'; tok.type = TOKEN_LOGIC_OP; return tok; }

    // Logic operator (one caractere)
    if (c == '<' || c == '>' || c == '!') {
        tok.type = TOKEN_LOGIC_OP;
        return tok;
    }

    // Arithmetic operator
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=') {
        tok.type = TOKEN_ARITHMETIC_OP;
        return tok;
    }

    // Separator
    if (c == '(' || c == ')' || c == '{' || c == '}' ||
        c == '[' || c == ']' || c == ';' || c == ',') {
        tok.type = TOKEN_SEPARATOR;
        return tok;
    }

    tok.type = TOKEN_UNKNOWN;

    return tok;
}