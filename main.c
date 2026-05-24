#include "lexer.h"
#include "parser.h"
#include "reader.h"

int main(int argc, char *argv[]) {
    char *src = read_file(argv[1]);
    if (!src) {
        printf("Error reading file\n");
        return 1;
    }

    // Lexer
    Lexer     l;
    lexer_init(&l, src);
    TokenList tl = lexer_tokenize(&l);

    // printf("=== TOKEN LIST ===\n");
    // token_list_print(&tl);
    // printf("\n");

    // Parser
    Parser   p;
    parser_init(&p, &tl);
    ASTNode *ast = parser_run(&p);
 
    printf("\n=== AST ===\n");
    ast_print(ast, 0);


    ast_free(ast);
    token_list_free(&tl);
    free_file(src);

    return 0;
}