#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

enum ast_type {
    TOPLEVEL_AST,
    FUNCTION_DEFINITION,
    RETURN_STATEMENT,
    AST_INTEGER,
    AST_ADD,
    AST_MINUS,
};

struct ast {
    enum ast_type type;

    union {
        // TOPLEVEL_AST
        struct {
            struct ast* asts[100];
            int ast_len;
        } toplevel;

        // RETURN_STATEMENT
        struct ast* exp;
        // INTEGER_LITERAL
        int integer;
        // FUNCTION_DEFINITION
        struct {
            char* function_name;
            int function_name_length;
            struct ast* body;
            // TODO: add return type
        } function_definition;

        struct {
            struct ast* left;
            struct ast* right;
        } binary_operator;
    } value;
};

struct state;
int parse(struct state*);

#endif
