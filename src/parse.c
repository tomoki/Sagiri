#include "parse.h"
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
    if (t->value.keyword != k) {
        fprintf(stderr, "keyword %d is expected, but got %d\n", k, t->value.keyword);
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
    if (t->value.punctuator != p) {
        fprintf(stderr, "punctuator %d is expected, but got %d\n", p, t->value.punctuator);
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

struct token* peek_n(struct cursor* c, int n)
{
    if (c->where + n < c->length)
        return &(c->tokens[c->where + n]);
    return NULL;
}

struct token* peek(struct cursor* c)
{
    return peek_n(c, 0);
}

void proceed(struct cursor* c)
{
    c->where++;
}

// --------------------- EXPRESSIONS -----------------------------
struct ast* expression(struct cursor*);

struct ast* integer_literal(struct cursor* c)
{
    struct token* integer_token = peek(c);
    proceed(c);
    struct ast* i = new_ast(AST_INTEGER);
    i->value.integer = integer_token->value.integer;
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

    if (next_token->type == TOKEN_IDENTIFIER) {
        proceed(c);
        struct ast* ret = new_ast(AST_IDENTIFIER);
        ret->value.identifier.name = next_token->value.identifier.start;
        ret->value.identifier.length = next_token->value.identifier.length;
        return ret;
    } else if (next_token->type == TOKEN_INTEGER)
        return integer_literal(c);
    else if (is_punctuator(next_token, PUNC_LEFT_PAREN)) {
        proceed(c); // (
        struct ast* ret = expression(c);
        expect_punctuator(peek(c), PUNC_RIGHT_PAREN);
        proceed(c); // )
        return ret;
    }
    error("Failed to parse primary_expression\n");
    return NULL; // surpress warning
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
    struct ast* left = multiplicative_expression(c);
    while (1) {
        struct token* op = peek(c);
        if (is_punctuator(op, PUNC_PLUS)) {
            proceed(c);
            struct ast* right = multiplicative_expression(c);
            struct ast* new_left = new_ast(AST_ADD);
            new_left->value.binary_operator.left = left;
            new_left->value.binary_operator.right = right;
            left = new_left;
        } else if (is_punctuator(op, PUNC_MINUS)) {
            proceed(c);
            struct ast* right = multiplicative_expression(c);
            struct ast* new_left = new_ast(AST_MINUS);
            new_left->value.binary_operator.left = left;
            new_left->value.binary_operator.right = right;
            left = new_left;
        } else
            break;
    }
    return left;
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


// ---------------- STATEMENT -----------------------------

struct ast* statement(struct cursor*);
struct ast* declaration(struct cursor*);

struct ast* labeled_statement(struct cursor* c)
{
    // identifier : statement
    // case constant-expression : statement
    // default : statement
    error("not implemented");
}

struct ast* compound_statement(struct cursor* c)
{
    // { block-item-list opt }
    // block-item-list:
    //    block-item
    //    block-item-list block-item
    // block-item:
    //    declaration
    //    statement

    struct ast* ret = new_ast(AST_COMPOUND_STATEMENT);
    ret->value.compound_statement.ast_len = 0;

    struct token* left_curly = peek(c);
    expect_punctuator(left_curly, PUNC_LEFT_CURLY);
    proceed(c);

    while (1) {
        struct token* next_token = peek(c);
        if (is_punctuator(next_token, PUNC_RIGHT_CURLY))
            break;

        // FIXME: allow tyoedef
        if (// storage-class-specifier
            is_keyword(next_token, KEY_TYPEDEF)  ||
            is_keyword(next_token, KEY_EXTERN)   ||
            is_keyword(next_token, KEY_STATIC)   ||
            is_keyword(next_token, KEY_AUTO)     ||
            is_keyword(next_token, KEY_REGISTER) ||
            // type-specifier
            is_keyword(next_token, KEY_VOID)     ||
            is_keyword(next_token, KEY_CHAR)     ||
            is_keyword(next_token, KEY_SHORT)    ||
            is_keyword(next_token, KEY_INT)      ||
            is_keyword(next_token, KEY_LONG)     ||
            is_keyword(next_token, KEY_FLOAT)    ||
            is_keyword(next_token, KEY_DOUBLE)   ||
            is_keyword(next_token, KEY_SIGNED)   ||
            is_keyword(next_token, KEY_UNSIGNED) ||
            is_keyword(next_token, KEY__BOOL)    ||
            is_keyword(next_token, KEY__COMPLEX) ||
            is_keyword(next_token, KEY_STRUCT)   ||
            is_keyword(next_token, KEY_UNION)    ||
            is_keyword(next_token, KEY_ENUM)     ||
            // or typedef name....
            // type-qualifier
            is_keyword(next_token, KEY_CONST)    ||
            is_keyword(next_token, KEY_RESTRICT) ||
            is_keyword(next_token, KEY_VOLATILE) ||
            // function-specifier
            is_keyword(next_token, KEY_INLINE))
            ret->value.compound_statement.asts[ret->value.compound_statement.ast_len++] = declaration(c);
        else
            ret->value.compound_statement.asts[ret->value.compound_statement.ast_len++] = statement(c);
    }

    struct token* right_curly = peek(c);
    expect_punctuator(right_curly, PUNC_RIGHT_CURLY);
    proceed(c);

    return ret;
}

struct ast* expression_statement(struct cursor* c)
{
    // expression opt;
    error("not implemented");
}

struct ast* selection_statement(struct cursor* c)
{
    // if ( expression ) statement
    // if ( expression ) statement else statement
    // switch ( expression ) statement
    struct token* first_token = peek(c);
    if (is_keyword(first_token, KEY_IF)) {
        struct ast* ret = new_ast(AST_IF_ELSE_STATEMENT);
        // if
        proceed(c);
        // (
        expect_punctuator(peek(c), PUNC_LEFT_PAREN);
        proceed(c);
        // expression
        ret->value.if_else_statement.cond_expression = expression(c);
        // )
        expect_punctuator(peek(c), PUNC_RIGHT_PAREN);
        proceed(c);
        ret->value.if_else_statement.true_statement = statement(c);

        struct token* next_token = peek(c);
        if (next_token != NULL && next_token->type == TOKEN_KEYWORD && next_token->value.keyword == KEY_ELSE) {
            proceed(c); // else
            ret->value.if_else_statement.false_statement = statement(c);
        } else
            ret->value.if_else_statement.false_statement = NULL;

        return ret;
    }
    // switch
    error("not implemented");
}

struct ast* iteration_statement(struct cursor* c)
{
    // while ( expression ) statement
    // do statement while ( expression ) ;
    // for ( expression opt ; expression opt ; expression opt ) statement
    // for ( declaration expression opt ; expression opt ) statement
    error("not implemented");
}

struct ast* jump_statement(struct cursor* c)
{
    // goto identifier ;
    // continue ;
    // break ;
    // return expression opt ;
    struct token* first_token = peek(c);
    if (is_keyword(first_token, KEY_RETURN)) {
        struct token* return_token = peek(c);
        expect_keyword(return_token, KEY_RETURN);
        proceed(c);

        struct ast* exp = expression(c);

        struct token* semicolon_token = peek(c);
        expect_punctuator(semicolon_token, PUNC_SEMICOLON);
        proceed(c);

        struct ast* ret = new_ast(RETURN_STATEMENT);
        ret->value.exp = exp;
        return ret;
    }

    error("not implemented");
}

struct ast* statement(struct cursor* c)
{
    // labeled-statement
    // compound-statement
    // expression-statement
    // selection-statement
    // iteration-statement
    // jump-statement

    struct token* first_token = peek(c);
    if (first_token == NULL)
        error("Reach EOF until parsing statement");

    // compound-statement
    if (is_punctuator(first_token, PUNC_LEFT_PAREN))
        return compound_statement(c);

    // jump-statement
    if (is_keyword(first_token, KEY_GOTO) || is_keyword(first_token, KEY_CONTINUE) ||
        is_keyword(first_token, KEY_BREAK) || is_keyword(first_token, KEY_RETURN))
        return jump_statement(c);

    // iteration-statement
    if (is_keyword(first_token, KEY_WHILE) || is_keyword(first_token, KEY_DO) || is_keyword(first_token, KEY_FOR))
        return iteration_statement(c);

    // selection-statement
    if (is_keyword(first_token, KEY_IF) || is_keyword(first_token, KEY_SWITCH))
        return selection_statement(c);

    // labeled-statement with keyword label
    if (is_keyword(first_token, KEY_CASE) || is_keyword(first_token, KEY_DEFAULT))
        return labeled_statement(c);
    // custom label?
    if (first_token->type == TOKEN_IDENTIFIER) {
        struct token* second_token = peek_n(c, 1);
        if (is_punctuator(second_token, PUNC_COLON))
            return labeled_statement(c);
    }

    // expression statement
    return expression_statement(c);
}

struct ast* declaration(struct cursor* c)
{
    // declaration-specifiers init-declarator-list opt ;

    // declaration-specifiers:
    //   storage-class-specifier declaration-specifiers opt
    //   type-specifier declaration-specifiers opt
    //   type-qualifier declaration-specifiers opt
    //   function-specifier declaration-specifiers opt

    // init-declarator-list:
    //   init-declarator
    //   init-declarator-list , init-declarator

    // init-declarator:
    //   declarator
    //   declarator = initializer


    // FIXME: Assume it is integer.
    expect_keyword(peek(c), KEY_INT);
    proceed(c);

    struct token* ident = peek(c);
    proceed(c);
    if (ident->type != TOKEN_IDENTIFIER)
        error("Found non identifier on declaration");

    struct ast* ret = new_ast(AST_DECLARATION);
    if (is_punctuator(peek(c), PUNC_EQUAL)) {
        proceed(c);
        ret->value.declaration.initializer = assignment_expression(c);
    } else
        ret->value.declaration.initializer = 0;

    expect_punctuator(peek(c), PUNC_SEMICOLON);
    proceed(c);
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

    // body
    struct ast* st = compound_statement(c);

    struct ast* ret = new_ast(FUNCTION_DEFINITION);
    ret->value.function_definition.function_name = function_name->value.identifier.start;
    ret->value.function_definition.function_name_length = function_name->value.identifier.length;
    ret->value.function_definition.body = st;

    return ret;
}

struct ast* translation_unit(struct cursor* c)
{
    struct ast* translation_unit = new_ast(TOPLEVEL_AST);
    while(peek(c) != NULL) {
        translation_unit->value.toplevel.asts[translation_unit->value.toplevel.ast_len++] = function_definition(c);
    }
    return translation_unit;
}

int parse(struct state* s)
{
    struct cursor c;
    c.tokens = s->tokens;
    c.where = 0;
    c.length = s->tokens_len;

    s->toplevel_ast = translation_unit(&c);
    return 0;
}
