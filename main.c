#include "lexer.h"
#include "parser.h"
#include "reader.h"

int main(int argc, char *argv[]) {
    char *src = read_file(argv[1]);
    if (!src) {
        printf("erro ao abrir arquivo\n");
        return 1;
    }

    Lexer  l;
    Parser p;
    lexer_init(&l, src);
    parser_init(&p, &l);
    parser_run(&p);

    free_file(src);

    return 0;
}