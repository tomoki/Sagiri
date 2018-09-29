#include "compile.h"

#include "parser.h"
#include "state.h"
#include "util.h"

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
        case AST_ADD:
            compile_rec(a->value.binary_operator.left, s);
            compile_rec(a->value.binary_operator.right, s);
            // left -> right -> stack top
            // rbx = right, rax = left
            printf("\tpopq %%rbx\n"
                   "\tpopq %%rax\n"
                   "\taddq %%rbx, %%rax\n"
                   "\tpushq %%rax\n");
            break;
        case AST_MINUS:
            compile_rec(a->value.binary_operator.left, s);
            compile_rec(a->value.binary_operator.right, s);
            // rbx = right, rax = left
            printf("\tpopq %%rbx\n"
                   "\tpopq %%rax\n"
                   "\tsubq %%rbx, %%rax\n"
                   "\tpushq %%rax\n");
            break;
        case AST_INTEGER:
            printf("\tpushq $%d\n", a->value.integer);
            break;
        case AST_COMPOUND_STATEMENT:
            for (int i = 0; i < a->value.compound_statement.ast_len; i++) {
                compile_rec(a->value.compound_statement.asts[i], s);
            }
            break;
        case AST_IF_ELSE_STATEMENT:
            compile_rec(a->value.if_else_statement.cond_expression, s);
            int fl = s->unique_label++;
            int el = s->unique_label++;
            printf("\tpopq %%rax\n"
                   "\tcmpq $0, %%rax\n"
                   "\tje .L%d\n", fl);
            compile_rec(a->value.if_else_statement.true_statement, s);
            printf("\tjmp .L%d\n", el);
            printf(".L%d:\n", fl);
            if (a->value.if_else_statement.false_statement)
                compile_rec(a->value.if_else_statement.false_statement, s);
            printf(".L%d:\n", el);
            break;
        default:
            error("not implemented");
    }
}

void compile_function(struct ast* a, struct state* s)
{
    for (int i = 0; i < a->value.function_definition.function_name_length; i++) {
        putc(a->value.function_definition.function_name[i], stdout);
    }
    printf(":\n");
    printf("\tpushq %%rbp\n"
           "\tmovq %%rsp, %%rbp\n"
    );
    compile_rec(a->value.function_definition.body, s);
}

void compile_toplevel(struct ast* a, struct state* s)
{
    printf(
        "\t.text\n"
        "\t.globl my_main\n");
    struct ast* top = s->toplevel_ast;
    for (int i = 0; i < top->value.toplevel.ast_len; i++) {
        struct ast* t = top->value.toplevel.asts[i];
        if (t->type == FUNCTION_DEFINITION)
            compile_function(t, s);
    }
}

int compile(struct state* s)
{
    s->unique_label = 1;
    compile_toplevel(s->toplevel_ast, s);
    return 0;
}
