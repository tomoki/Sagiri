#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

enum ast_type {
    RETURN_STATEMENT,
    INTEGER_LITERAL
};

struct ast {
    enum ast_type type;

    union {
        // RETURN_STATEMENT
        struct ast* exp;
        // INTEGER_LITERAL
        int integer_value;
    } value;
};

struct state;
int parse(struct state*);

#endif
