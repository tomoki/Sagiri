#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

struct token;
struct ast;

struct state {
    char* source;
    int source_len;

    struct token *tokens;
    int tokens_len;

    // FIXME: How should we store it?
    struct ast *asts;
};

#endif
