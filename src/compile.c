#include "compile.h"

#include "parser.h"
#include "state.h"

#include <stdio.h>

void compile_rec(struct ast* a, struct state* s)
{
    switch (a->type) {
        case RETURN_STATEMENT:
            compile_rec(a->value.exp, s);
            printf(
                "\tpopq %%rax\n"
                "\tpopq %%rbp\n"
                "\tret\n"
            );
            break;
        case INTEGER_LITERAL:
            printf("\tpushq $%d\n", a->value.integer_value);
            break;
    }
}

int compile(struct state* s)
{
    printf(
        "\t.text\n"
        "\t.globl my_main\n"
        "my_main:\n"
        "\tpushq %%rbp\n"
        "\tmovq %%rsp, %%rbp\n"
    );

    compile_rec(s->asts, s);
    return 0;
}
