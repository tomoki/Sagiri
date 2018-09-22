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

void compile_function(struct ast* a, struct state* s)
{
    for (int i = 0; i < a->value.function_definition_value.function_name_length; i++) {
        putc(a->value.function_definition_value.function_name[i], stdout);
    }
    printf(":\n");
    printf("\tpushq %%rbp\n"
           "\tmovq %%rsp, %%rbp\n"
    );
    compile_rec(a->value.function_definition_value.body, s);
}

void compile_toplevel(struct ast* a, struct state* s)
{
    printf(
        "\t.text\n"
        "\t.globl my_main\n");
    struct ast* top = s->toplevel_ast;
    for (int i = 0; i < top->value.toplevel_value.ast_len; i++) {
        struct ast* t = top->value.toplevel_value.asts[i];
        if (t->type == FUNCTION_DEFINITION)
            compile_function(t, s);
    }
}

int compile(struct state* s)
{
    compile_toplevel(s->toplevel_ast, s);
    return 0;
}
