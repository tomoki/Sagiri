#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

enum ast_type {
    TOPLEVEL_AST,
    FUNCTION_DEFINITION,
    RETURN_STATEMENT,
    AST_INTEGER,
    AST_IDENTIFIER,
    AST_ADD,
    AST_MINUS,
    AST_COMPOUND_STATEMENT,
    AST_IF_ELSE_STATEMENT,
    AST_DECLARATION,
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
            char* name;
            int length;
        } identifier;

        // FUNCTION_DEFINITION
        struct {
            char* function_name;
            int function_name_length;
            struct ast* body;
            // TODO: add return type
        } function_definition;

        // DECLARATION
        struct {
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
