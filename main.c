#include "lexer.h"
#include "reader.h"

int main(int argc, char *argv[]) {
    char *src = read_file(argv[1]);
    if (!src) {
        printf("erro ao abrir arquivo\n");
        return 1;
    }

    Lexer l;
    lexer_init(&l, src);

    Token tok;
    tok = next_token(&l);

    while (tok.type != TOKEN_EOF){
        print_token(&tok);
        tok = next_token(&l);
    }

    free_file(src);

    return 0;
}