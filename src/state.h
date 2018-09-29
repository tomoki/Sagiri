#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

struct token;
struct ast;

struct state {
    char *source;
    int source_len;

    struct token *tokens;
    int tokens_len;

    struct ast *toplevel_ast;
    int unique_label;
};

#endif
