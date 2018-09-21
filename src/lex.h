#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

enum token_type {
    INTEGER_TOKEN,
    RETURN_TOKEN,
    SEMICOLON_TOKEN
};

struct token {
    enum token_type type;
    union value_type {
        int integer_value;
    } value;
};

struct state;
int lex(struct state*);

#endif
