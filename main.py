import lexer as lex_module

if __name__ == "__main__":
    with open("text/input.txt", "r") as f:
        code = f.read()

    _lexer = lex_module.lexer
    _lexer.input(code)

    with open("text/tokens.txt", "w") as f:
        for tok in _lexer:
            f.write(str(tok) + "\n")

    for tok in _lexer:
        print(tok)