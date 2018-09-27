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
