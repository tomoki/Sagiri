#include "lex.h"

#include "state.h"

#include <stdlib.h>

int lex(struct state* s)
{
    // Just for testing
    // return 1;
    s->tokens = malloc(sizeof(struct token) * 10);
    s->tokens_len = 3;
    s->tokens[0].type = RETURN_TOKEN;
    s->tokens[1].type = INTEGER_TOKEN;
    s->tokens[1].value.integer_value = 42;
    s->tokens[2].type = SEMICOLON_TOKEN;
    return 0;
}
