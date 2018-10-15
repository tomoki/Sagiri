#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

enum ast_type {
    AST_TOPLEVEL,
    AST_FUNCTION_DEFINITION,
    AST_RETURN_STATEMENT,
    AST_INTEGER,
    AST_IDENTIFIER,
    AST_ADD,
    AST_MINUS,
    AST_COMPOUND_STATEMENT,
    AST_IF_ELSE_STATEMENT,
    AST_DECLARATION,
};

struct identifier {
    char* name;
    int length;
};

struct ast {
    enum ast_type type;

    union {
        // compound statement
        struct {
            struct ast* asts[100];
            int ast_len;
        } compound_statement;

        // if-else statement
        struct {
            struct ast* cond_expression;
            struct ast* true_statement;
            struct ast* false_statement;
        } if_else_statement;

        struct {
            struct ast* asts[100];
            int ast_len;
        } toplevel;

        struct ast* exp;

        // INTEGER_LITERAL
        int integer;

        // IDENTIFIER REFERENCE
        struct {
            struct identifier identifier;
            int type;
            int var_index; // in function
        } identifier_reference;

        // FUNCTION_DEFINITION
        struct {
            struct identifier function_name;
            struct ast* body;
            // TODO: add return type
            int number_of_vars;
        } function_definition;

        // DECLARATION
        struct {
            // FIXME: should be multiple identifier
            struct identifier identifier;
            int var_index;
            struct ast* initializer;
        } declaration;

        struct {
            struct ast* left;
            struct ast* right;
        } binary_operator;
    } value;
};

struct state;
int parse(struct state*);

#endif
