import ply.lex as lex

# RESERVED WORDS

reserved_words = {
    # data type
    'int', 'float', 'double', 'char', 'void', 'long', 'short',
    'unsigned', 'signed',

    # flow control
    'if', 'else', 'switch', 'case', 'default',
    'while', 'for', 'do',
    'break', 'continue', 'return', 'goto',

    # structeres
    'struct', 'union', 'enum', 'typedef',

    # storage modifications
    'auto', 'static', 'extern', 'register', 'volatile', 'const',

    # others
    'sizeof',
}

# TOKENS

tokens = (
    'IDENTIFIER',
    'RESERVED_WORD',
    'NUMBER',
    'LITERALS',
    'ASSIGN',
    'LOGIC_OPERATOR',
    'ARITHMETIC_OPERATOR',
    'SEPARATOR',
)

# SIMPLE RULES

t_ARITHMETIC_OPERATOR  = r'[+\-*/]'
t_LOGIC_OPERATOR       = r'==|!=|<=|>=|<|>|&&|\|\|'
t_ASSIGN               = r'='
t_SEPARATOR            = r'[(),;{}[\]]'
t_LITERALS             = r'\".*?\"|\'.*?\''

# COMPLEX RULES

def t_COMMENT(t):
    r'//.*|/\*[\s\S]*?\*/'
    pass

def t_NUMBER(t):
    r'\d+(\.\d+)?'
    t.value = float(t.value) if '.' in t.value else int(t.value)
    return t

def t_IDENTIFIER(t):
    r'[a-zA-Z_][a-zA-Z0-9_]*'
    if t.value in reserved_words:
        t.type = 'RESERVED_WORD'
    return t

# IGNORED CHARACTERS AND TABS

t_ignore = ' \t'

def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)

def t_error(t):
    print(f"Caractere inválido: '{t.value[0]}' na linha {t.lexer.lineno}")
    t.lexer.skip(1)

# BUILD

lexer = lex.lex()