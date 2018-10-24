#include "compile.h"

#include "parse.h"
#include "state.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// index is when the variable appears in function local variable.
struct environment {
    struct environment* parent;

    struct identifier keys[100];
    int indexes[100];
    int types[100];
    int defines;
};

struct environment* new_environment(struct environment* parent)
{
    struct environment* ret = malloc(sizeof(struct environment));
    memset(ret, 0, sizeof(struct environment));
    ret->parent = parent;
    return ret;
}

int refer_identifier(struct environment* env, struct identifier ident, int *var_index)
{
    if (!env)
        error("refer_identifier got null env");

    for (int i = 0; i < env->defines; i++) {
        int ilen = ident.length >= env->keys[i].length ? ident.length : env->keys[i].length;
        if (strncmp(ident.name, env->keys[i].name, ilen) == 0) {
            *var_index = env->indexes[i];
            return 1;
        }
    }

    if (env->parent)
        return refer_identifier(env->parent, ident, var_index);
    return 0;
}

void scan_variables(struct ast* a, struct environment* env, int* var_index, struct state* s)
{
    switch (a->type) {
        case AST_RETURN_STATEMENT:
        case AST_EXPRESSION_STATEMENT:
            scan_variables(a->value.exp, env, var_index, s);
            break;

        // binary operators
        case AST_ADD:
        case AST_MINUS:
            scan_variables(a->value.binary_operator.left, env, var_index, s);
            scan_variables(a->value.binary_operator.right, env, var_index, s);
            break;

        case AST_IF_ELSE_STATEMENT:
            scan_variables(a->value.if_else_statement.cond_expression, env, var_index, s);
            scan_variables(a->value.if_else_statement.true_statement,  env, var_index, s);
            if (a->value.if_else_statement.false_statement)
                scan_variables(a->value.if_else_statement.false_statement, env, var_index, s);
            break;

        case AST_COMPOUND_STATEMENT: {
            struct environment* e = new_environment(env);
            for(int i = 0; i < vector_length(a->value.compound_statement.asts); i++)
                scan_variables(*(struct ast**) vector_at(a->value.compound_statement.asts, i), e, var_index, s);
            break;
        }

        case AST_DECLARATION: {
            env->keys[env->defines] = a->value.declaration.identifier;
            env->indexes[env->defines] = *var_index;
            a->value.declaration.var_index = *var_index;
            env->defines++;
            (*var_index)++;
            break;
        }

        case AST_IDENTIFIER: {
            int index = 0;
            int found = refer_identifier(env, a->value.identifier_reference.identifier, &index);
            if (!found)
                error("Identifier not found");
            a->value.identifier_reference.var_index = index;
            break;
        }

        case AST_FUNCTION_CALL: {
            // FIXME: Should scan functions
        }

        // primitives other than identifier, do nothing
        case AST_INTEGER:
            break;

        // scan_variables only scan function definition.
        case AST_TOPLEVEL:
        case AST_FUNCTION_DEFINITION:
            error("should not reach");
            break;

        default:
            error("not implemented");
            break;
    }
    return;
}

char parameter_registers[][256] = {
    "rdi", "rsi", "rdx", "rcx", "r8", "r9"
};

void scan_variables_toplevel(struct state* s)
{
    struct ast* top = s->toplevel_ast;
    struct environment* global_env = new_environment(0);

    for (int i = 0; i < vector_length(top->value.toplevel.asts); i++) {
        struct ast* t = *(struct ast**) vector_at(top->value.toplevel.asts, i);
        if (t->type == AST_FUNCTION_DEFINITION) {
            struct environment* function_env = new_environment(global_env);
            for (int i = 0; i < vector_length(t->value.function_definition.parameters); i++) {
                function_env->keys[function_env->defines] = *(struct identifier*) vector_at(t->value.function_definition.parameters, i);
                function_env->indexes[function_env->defines] = i;
                function_env->defines++;
            }
            t->value.function_definition.number_of_vars = vector_length(t->value.function_definition.parameters);
            // TODO: scan arguments
            // FIXME: body should not new have environment
            scan_variables(t->value.function_definition.body, function_env, &(t->value.function_definition.number_of_vars), s);
        }
    }
}

void compile_rec(struct ast* a, struct ast* function, struct state* s)
{
    switch (a->type) {
        case AST_RETURN_STATEMENT:
            compile_rec(a->value.exp, function, s);
            printf("\tpopq %%rax\n");
            // remove locals
            printf("\taddq $%d, %%rsp\n", function->value.function_definition.number_of_vars*8);
            printf("\tpopq %%rbp\n"
                   "\tret\n"
            );
            break;
        case AST_ADD:
            compile_rec(a->value.binary_operator.left, function, s);
            compile_rec(a->value.binary_operator.right, function, s);
            // left -> right -> stack top
            // rbx = right, rax = left
            printf("\tpopq %%rbx\n"
                   "\tpopq %%rax\n"
                   "\taddq %%rbx, %%rax\n"
                   "\tpushq %%rax\n");
            break;
        case AST_MINUS:
            compile_rec(a->value.binary_operator.left, function, s);
            compile_rec(a->value.binary_operator.right, function, s);
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
            for (int i = 0; i < vector_length(a->value.compound_statement.asts); i++) {
                compile_rec(*(struct ast**)vector_at(a->value.compound_statement.asts, i), function, s);
            }
            break;
        case AST_IF_ELSE_STATEMENT:
            compile_rec(a->value.if_else_statement.cond_expression, function, s);
            int fl = s->unique_label++;
            int el = s->unique_label++;
            printf("\tpopq %%rax\n"
                   "\tcmpq $0, %%rax\n"
                   "\tje .L%d\n", fl);
            compile_rec(a->value.if_else_statement.true_statement, function, s);
            printf("\tjmp .L%d\n", el);
            printf(".L%d:\n", fl);
            if (a->value.if_else_statement.false_statement)
                compile_rec(a->value.if_else_statement.false_statement, function, s);
            printf(".L%d:\n", el);
            break;
        case AST_DECLARATION:
            if (a->value.declaration.initializer) {
                compile_rec(a->value.declaration.initializer, function, s);
                printf("\tpopq %%rax\n"
                       "\tmovq %%rax, -%d(%%rbp)\n", (a->value.declaration.var_index+1)*8);
            }
            break;
        case AST_FUNCTION_CALL:
            if (a->value.function_call.function->type == AST_IDENTIFIER) {
                // push arguments
                for (int i = 0; i < vector_length(a->value.function_call.arguments); i++) {
                    struct ast* arg = *(struct ast**) vector_at(a->value.function_call.arguments, i);
                    compile_rec(arg, function, s);
                    printf("\tpopq %%rax\n");
                    printf("\tmovq %%rax, %%%s\n", parameter_registers[i]);
                }
                char name[256];
                strncpy(name,
                        a->value.function_call.function->value.identifier_reference.identifier.name,
                        a->value.function_call.function->value.identifier_reference.identifier.length);
                name[a->value.function_call.function->value.identifier_reference.identifier.length] = '\0';
                printf("\tcall %s\n", name);
                printf("\tpushq %%rax\n");
                break;
            }
            error("not implemented");
        case AST_IDENTIFIER:
            printf("\tpushq -%d(%%rbp)\n", (a->value.identifier_reference.var_index+1)*8);
            break;
        case AST_EXPRESSION_STATEMENT:
            compile_rec(a->value.exp, function, s);
            printf("\tpopq %%rax\n");
            break;
        default:
            error("not implemented");
    }
}

void compile_function(struct ast* a, struct state* s)
{
    for (int i = 0; i < a->value.function_definition.function_name.length; i++)
        putc(a->value.function_definition.function_name.name[i], stdout);

    printf(":\n");
    printf("\tpushq %%rbp\n"
           "\tmovq %%rsp, %%rbp\n");

    // copy arguments
    for (int i = 0; i < vector_length(a->value.function_definition.parameters); i++)
        printf("\tmovq %%%s, -%d(%%rbp)\n", parameter_registers[i], (i+1) * 8);

    printf("\tsubq $%d, %%rsp\n", a->value.function_definition.number_of_vars * 8);
    compile_rec(a->value.function_definition.body, a, s);
}

void compile_toplevel(struct state* s)
{
    struct ast* top = s->toplevel_ast;
    printf("\t.text\n");
    for (int i = 0; i < vector_length(top->value.toplevel.asts); i++) {
        struct ast* t = *(struct ast**)vector_at(top->value.toplevel.asts, i);
        if (t->type == AST_FUNCTION_DEFINITION) {
            char name[256];
            strncpy(name, t->value.function_definition.function_name.name, t->value.function_definition.function_name.length);
            name[t->value.function_definition.function_name.length] = '\0';
            printf("\t.globl %s\n", name);
        }
    }

    for (int i = 0; i < vector_length(top->value.toplevel.asts); i++) {
        struct ast* t = *(struct ast**)vector_at(top->value.toplevel.asts, i);
        if (t->type == AST_FUNCTION_DEFINITION)
            compile_function(t, s);
    }
}

int compile(struct state* s)
{
    s->unique_label = 1;
    scan_variables_toplevel(s);
    compile_toplevel(s);
    return 0;
}
