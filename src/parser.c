#include "parser.h"
#include "lex.h"

#include "state.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void expect(struct token* t, enum token_type e)
{
    if (t == NULL) {
        fprintf(stderr, "Token is NULL");
        error("");
    }
    if (t->type != e) {
        fprintf(stderr, "%d is expected, but got %d\n", e, t->type);
        error("");
    }
}

void expect_keyword(struct token* t, enum keyword k)
{
    if (t == NULL) {
        fprintf(stderr, "Token is NULL");
        error("");
    }
    if (t->type != TOKEN_KEYWORD) {
        fprintf(stderr, "keyword is expected, but got %d\n", t->type);
        error("");
    }
    if (t->value.keyword_value != k) {
        fprintf(stderr, "keyword %d is expected, but got %d\n", k, t->value.keyword_value);
        error("");
    }
}

void expect_punctuator(struct token* t, enum punctuator p)
{
    if (t == NULL) {
        fprintf(stderr, "Token is NULL");
        error("");
    }
    if (t->type != TOKEN_PUNCTUATOR) {
        fprintf(stderr, "punctuator is expected, but got %d\n", t->type);
        error("");
    }
    if (t->value.punctuator_value != p) {
        fprintf(stderr, "punctuator %d is expected, but got %d\n", p, t->value.punctuator_value);
        error("");
    }
}

struct cursor
{
    struct token* tokens;
    int where;
    int length;
};

struct ast* new_ast(enum ast_type type)
{
    struct ast* ret = malloc(sizeof(struct ast));
    ret->type = type;
    return ret;
}

struct token* peek(struct cursor* c)
{
    if (c->where < c->length)
        return &(c->tokens[c->where]);
    return NULL;
}

void proceed(struct cursor* c)
{
    c->where++;
}

struct ast* primary_expression(struct cursor*);
struct ast* expression(struct cursor*);
struct ast* return_statement(struct cursor*);
struct ast* integer_literal(struct cursor*);

struct ast* integer_literal(struct cursor* c)
{
    struct token* integer_token = peek(c);
    proceed(c);
    struct ast* i = new_ast(INTEGER_LITERAL);
    i->value.integer_value = integer_token->value.integer_value;
    return i;
}

struct ast* primary_expression(struct cursor* c)
{
    // identifier
    // constant
    // string-literal
    // ( expression)
    struct token* next_token = peek(c);
    if (next_token == NULL)
        error("EOF until parsing expression\n");

    if (next_token->type == TOKEN_IDENTIFIER)
        error("not implemented");
    else if (next_token->type == TOKEN_INTEGER)
        return integer_literal(c);
    else if (next_token->type == TOKEN_PUNCTUATOR && next_token->value.punctuator_value == PUNC_LEFT_PAREN) {
        proceed(c); // ()
        struct ast* ret = expression(c);
        expect_punctuator(peek(c), PUNC_RIGHT_PAREN);
        proceed(c);
        return ret;
    }
    error("Failed to parse primary_expression\n");
}

struct ast* postfix_expression(struct cursor* c)
{
    // primary-expression
    // postfix-expression [ expression ]
    // postfix-expression ( argument-expression-list opt )
    // postfix-expression . identifier
    // postfix-expression -> identifier
    // postfix-expression ++
    // postfix-expression --
    // ( type-name ) { initializer-list }
    // ( type-name ) { initializer-list , }
    return primary_expression(c);
}

struct ast* unary_expression(struct cursor* c)
{
    // postfix-expression
    // ++ unary-expression
    // -- unary-expression
    // unary-operator cast-expression
    // sizeof unary-expression
    // sizeof ( type-name )
    return postfix_expression(c);
}

struct ast* cast_expression(struct cursor* c)
{
    // unary-expression
    // ( type-name ) cast-expression
    return unary_expression(c);
}

struct ast* multiplicative_expression(struct cursor* c)
{
    // cast-expression
    // multiplicative-expression * cast-expression
    // multiplicative-expression / cast-expression
    // multiplicative-expression % cast-expression
    return cast_expression(c);
}

struct ast* additive_expression(struct cursor* c)
{
    // multiplicative-expression
    // additive-expression + multiplicative-expression
    // additive-expression - multiplicative-expression
    return multiplicative_expression(c);
}

struct ast* shift_expression(struct cursor* c)
{
    // additive-expression
    // shift-expression << additive-expression
    // shift-expression >> additive-expression
    return additive_expression(c);
}

struct ast* relational_expression(struct cursor* c)
{
    // shift-expression
    // relational-expression < shift-expression
    // relational-expression > shift-expression
    // relational-expression <= shift-expression
    // relational-expression >= shift-expression
    return shift_expression(c);
}

struct ast* equality_expression(struct cursor* c)
{
    // relational-expression
    // equality-expression == relational-expression
    // equality-expression != relational-expression
    return relational_expression(c);
}

struct ast* and_expression(struct cursor* c)
{
    // equality-expression
    // AND-expression & equality-expression
    return equality_expression(c);
}

struct ast* exclusive_or_expression(struct cursor* c)
{
    // AND-expression
    // exclusive-OR-expression ^ AND-expression
    return and_expression(c);
}

struct ast* inclusive_or_expression(struct cursor* c)
{
    // exclusive-OR-expression
    // inclusive-OR-expression | exclusive-OR-expression
    return exclusive_or_expression(c);
}

struct ast* logical_and_expression(struct cursor* c)
{
    // inclusive-OR-expression
    // logical-AND-expression && inclusive-OR-expression
    return inclusive_or_expression(c);
}

struct ast* logical_or_expression(struct cursor* c)
{
    // logical-AND-expression
    // logical-OR-expression || logical-AND-expression
    return logical_and_expression(c);
}

struct ast* conditional_expression(struct cursor* c)
{
    // logical-OR-expression
    // logical-OR-expression ? expression : conditional-expression
    return logical_or_expression(c);
}

struct ast* assignment_expression(struct cursor* c)
{
    // conditional-expression
    // unary-expression assignment-operator assignment-expression
    return conditional_expression(c);
}

struct ast* expression(struct cursor* c)
{
    // assignment-expression
    // expression , assignment-expression
    return assignment_expression(c);
}

struct ast* return_statement(struct cursor* c)
{
    struct token* return_token = peek(c);
    expect_keyword(return_token, KEY_RETURN);
    proceed(c);

    // struct ast* exp = expression(c);
    struct ast* exp = integer_literal(c);
    if (exp == NULL)
        error("return exp");

    struct token* semicolon_token = peek(c);
    expect_punctuator(semicolon_token, PUNC_SEMICOLON);
    proceed(c);

    struct ast* ret = new_ast(RETURN_STATEMENT);
    ret->value.exp = exp;
    return ret;
}

struct ast* function_definition(struct cursor* c)
{
    // return type
    struct token* return_type = peek(c);
    // FIXME: identifier or keyword?
    proceed(c);

    struct token* function_name = peek(c);
    expect(function_name, TOKEN_IDENTIFIER);
    proceed(c);

    // (
    struct token* left_paren = peek(c);
    expect_punctuator(left_paren, PUNC_LEFT_PAREN);
    proceed(c);

    // )
    struct token* right_paren = peek(c);
    expect_punctuator(right_paren, PUNC_RIGHT_PAREN);
    proceed(c);

    // {
    struct token* left_curly = peek(c);
    expect_punctuator(left_curly, PUNC_LEFT_CURLY);
    proceed(c);

    // body
    struct ast* statement = return_statement(c);

    // }
    struct token* right_curly = peek(c);
    expect_punctuator(right_curly, PUNC_RIGHT_CURLY);
    proceed(c);

    struct ast* ret = new_ast(FUNCTION_DEFINITION);
    ret->value.function_definition_value.function_name = function_name->value.identifier_value.start;
    ret->value.function_definition_value.function_name_length = function_name->value.identifier_value.length;
    ret->value.function_definition_value.body = statement;

    return ret;
}

struct ast* toplevel(struct cursor* c)
{
    struct ast* top = new_ast(TOPLEVEL_AST);
    top->value.toplevel_value.ast_len = 1;
    top->value.toplevel_value.asts[0] = function_definition(c);
    return top;
}

int parse(struct state* s)
{
    struct cursor c;
    c.tokens = s->tokens;
    c.where = 0;
    c.length = s->tokens_len;

    s->toplevel_ast = toplevel(&c);
    return 0;
}
