#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

enum token_type {
    IDENTIFIER_TOKEN,
    INTEGER_TOKEN,
    RETURN_TOKEN,

    SEMICOLON_TOKEN,
    LEFTPAREN_TOKEN,
    RIGHTPAREN_TOKEN,
    LEFTCURLY_TOKEN,
    RIGHTCURLY_TOKEN
};

struct token {
    enum token_type type;
    union value_type {
        int integer_value;
        struct {
            char* start;
            int length;
        } identifier_value;
    } value;
};

struct state;
int lex(struct state*);

#endif
