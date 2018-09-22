#include "lex.h"

#include "state.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cursor
{
    char* code;
    int where;
    int length;
};

// return the identifier length.
int read_identifier(struct cursor* c)
{
    int l = 0;
    int i = c->where;
    char first = c->code[i];
    if (isalpha(first))
        while (isalnum(c->code[i+l]) || c->code[i+l] == '_')
            l++;

    return l;
}

// return the integer length.
int read_integer(struct cursor* c)
{
    int l = 0;
    int i = c->where;
    while (isdigit(c->code[i+l]))
        l++;
    return l;
}

int do_lex(struct cursor* c, struct state* s)
{
    int k = 0;
    while (c->code[c->where] != '\0') {
        fprintf(stderr, "%s\n", c->code + c->where);
        // Skip spaces
        while (isspace(c->code[c->where]))
            c->where++;

        // try identifier
        int l = read_identifier(c);
        if (l != 0) {
            // check keywords
            if (strncmp(&c->code[c->where], "return", l) == 0) {
                c->where += l;
                s->tokens[k].type = RETURN_TOKEN;
                k++;
                continue;
            }
            // Normal identifier
            s->tokens[k].type = IDENTIFIER_TOKEN;
            s->tokens[k].value.identifier_value.start = &(c->code[c->where]);
            s->tokens[k].value.identifier_value.length = l;
            k++;
            c->where += l;
            continue;
        }
        l = read_integer(c);
        if (l != 0) {
            int n = 0;
            for (int j = 0; j < l; j++)
                n = 10 * n + (c->code[c->where+j]-'0');
            c->where += l;
            s->tokens[k].type = INTEGER_TOKEN;
            s->tokens[k].value.integer_value = n;
            k++;
            continue;
        }
        if (c->code[c->where] == ';') {
            s->tokens[k].type = SEMICOLON_TOKEN;
            k++;
            c->where++;
            continue;
        }
        if (c->code[c->where] == '(') {
            s->tokens[k].type = LEFTPAREN_TOKEN;
            k++;
            c->where++;
            continue;
        }
        if (c->code[c->where] == ')') {
            s->tokens[k].type = RIGHTPAREN_TOKEN;
            k++;
            c->where++;
            continue;
        }
        if (c->code[c->where] == '{') {
            s->tokens[k].type = LEFTCURLY_TOKEN;
            k++;
            c->where++;
            continue;
        }
        if (c->code[c->where] == '}') {
            s->tokens[k].type = RIGHTCURLY_TOKEN;
            k++;
            c->where++;
            continue;
        }
    }
    s->tokens_len = k;
}

int lex(struct state* s)
{
    struct cursor c;
    c.code = s->source;
    c.where = 0;
    c.length = s->source_len;
    s->tokens = malloc(sizeof(struct token) * 10);
    do_lex(&c, s);

    // for (int i = 0; i < s->tokens_len; i++) {
    //     fprintf(stderr, "%d -> type = %d, value = %d\n", i, s->tokens[i].type, s->tokens[i].value.integer_value);
    // }

    return 0;
}
