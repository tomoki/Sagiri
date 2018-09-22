#include "parser.h"
#include "lex.h"

#include "state.h"

#include <stdio.h>
#include <stdlib.h>

struct cursor
{
    struct token* tokens;
    int where;
    int length;
};

struct ast* new_ast(enum ast_type type)
{
    struct ast* ret = malloc(sizeof(struct ast));
    ret->type = type;
    return ret;
}

struct token* peek(struct cursor* c)
{
    if (c->where < c->length)
        return &(c->tokens[c->where]);
    return NULL;
}

void proceed(struct cursor* c)
{
    c->where++;
}

struct ast* expression(struct cursor*);
struct ast* return_statement(struct cursor*);
struct ast* integer_literal(struct cursor*);

struct ast* integer_literal(struct cursor* c)
{
    struct token* integer_token = peek(c);
    proceed(c);
    struct ast* i = new_ast(INTEGER_LITERAL);
    i->value.integer_value = integer_token->value.integer_value;
    return i;
}

struct ast* return_statement(struct cursor* c)
{
    struct token* return_token = peek(c);
    if (return_token == NULL || return_token->type != RETURN_TOKEN)
        return NULL;

    proceed(c);

    // struct ast* exp = expression(c);
    struct ast* exp = integer_literal(c);
    if (exp == NULL)
        return NULL;

    struct token* semicolon_token = peek(c);
    if (semicolon_token == NULL || semicolon_token->type != SEMICOLON_TOKEN)
        return NULL;
    proceed(c);

    struct ast* ret = new_ast(RETURN_STATEMENT);
    ret->value.exp = exp;
}

int parse(struct state* s)
{

    struct cursor c;
    c.tokens = s->tokens;
    c.where = 0;
    c.length = s->tokens_len;

    s->asts = return_statement(&c);
    return 0;
}
