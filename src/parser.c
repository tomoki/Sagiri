#include "parser.h"

#include "state.h"

#include <stdlib.h>

int parse(struct state* s)
{
    struct ast *ret_val = malloc(sizeof(struct ast));
    ret_val->type = INTEGER_LITERAL;
    ret_val->value.integer_value = 42;

    struct ast *ret = malloc(sizeof(struct ast));
    ret->type = RETURN_STATEMENT;
    ret->value.exp = ret_val;

    s->asts = ret;
    return 0;
}
