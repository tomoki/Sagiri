#include "lex.h"

#include "state.h"
#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cursor
{
    char* code;
    int where;
    int length;
};

#define NUMBER_OF_KEYWORDS 37

char keywords[NUMBER_OF_KEYWORDS][10] =  {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "inline",
    "int",
    "long",
    "register",
    "restrict",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
    "_bool",
    "_complex",
    "_imaginary"
};

#define NUMBER_OF_PUNCTUATORS 53
char punctuators[NUMBER_OF_PUNCTUATORS][10] = {
    "%:%:",
    ">>=",
    "<<=",
    "...",
    "%:",
    "%>",
    "<%",
    ":>",
    "<:",
    "##",
    "|=",
    "^=",
    "&=",
    "-=",
    "+=",
    "%=",
    "/=",
    "*=",
    "||",
    "&&",
    "!=",
    "==",
    ">=",
    "<=",
    ">>",
    "<<",
    "--",
    "++",
    "#",
    ",",
    "=",
    ";",
    ":",
    "?",
    "|",
    "^",
    ">",
    "<",
    "%",
    "/",
    "!",
    "~",
    "-",
    "+",
    "*",
    "&",
    ".",
    "}",
    "{",
    ")",
    "(",
    "]",
    "[",
};


int is_keyword(struct token* t, enum keyword k)
{
     return t != NULL && t->type == TOKEN_KEYWORD && t->value.keyword == k;
}

int is_punctuator(struct token* t, enum punctuator p)
{
     return t != NULL && t->type == TOKEN_PUNCTUATOR && t->value.punctuator == p;
}

int nondigit(char c)
{
    return c == '_' ||
           ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z');
}

int digit(char c)
{
    return ('0' <= c) && (c <= '9');
}

// return the identifier length.
int read_identifier(struct cursor* c)
{
    int l = 0;
    int i = c->where;
    if (nondigit(c->code[i]))
        while (nondigit(c->code[i+l]) || digit(c->code[i+l]))
            l++;

    return l;
}

// return the integer length.
int read_integer(struct cursor* c)
{
    int l = 0;
    int i = c->where;
    while (isdigit(c->code[i+l]))
        l++;
    return l;
}

int do_lex(struct cursor* c, struct state* s)
{
    int k = 0;
    while (c->code[c->where] != '\0') {
        // Skip spaces
        while (isspace(c->code[c->where]))
            c->where++;

        {
            // try identifier
            int ident_len = read_identifier(c);
            if (ident_len != 0) {
                int is_keyword = 0;
                for (int i = 0; i < NUMBER_OF_KEYWORDS; i++) {
                    char *key = keywords[i];
                    int key_len = strlen(key);
                    if (key_len == ident_len && strncmp(&c->code[c->where], key, key_len) == 0) {
                        c->where += ident_len;
                        s->tokens[k].type = TOKEN_KEYWORD;
                        s->tokens[k].value.keyword = i;
                        k++;
                        is_keyword = 1;
                        break;
                    }
                }
                if (!is_keyword) {
                    // Normal identifier
                    s->tokens[k].type = TOKEN_IDENTIFIER;
                    s->tokens[k].value.identifier.start = &(c->code[c->where]);
                    s->tokens[k].value.identifier.length = ident_len;
                    k++;
                    c->where += ident_len;
                }
                continue;
            }
        }
        {
            int int_len = read_integer(c);
            if (int_len != 0) {
                int n = 0;
                for (int j = 0; j < int_len; j++)
                    n = 10 * n + (c->code[c->where+j]-'0');
                c->where += int_len;
                s->tokens[k].type = TOKEN_INTEGER;
                s->tokens[k].value.integer = n;
                k++;
                continue;
            }
        }
        {
            int is_punctuator = 0;
            // punctators
            for (int i = 0; i < NUMBER_OF_PUNCTUATORS; i++) {
                char *punc = punctuators[i];
                int punc_len = strlen(punc);
                if (strncmp(&c->code[c->where], punc, punc_len) == 0) {
                    c->where += punc_len;
                    s->tokens[k].type = TOKEN_PUNCTUATOR;
                    s->tokens[k].value.keyword = i;
                    k++;
                    is_punctuator = 1;
                }
            }
            if (is_punctuator)
                continue;
        }

        if (c->code[c->where] != '\0') {
            fprintf(stderr, "%s was not eaten\n", &c->code[c->where]);
            error("");
        }
    }
    s->tokens_len = k;
    return 0;
}

int lex(struct state* s)
{
    struct cursor c;
    c.code = s->source;
    c.where = 0;
    c.length = s->source_len;
    s->tokens = malloc(sizeof(struct token) * 100);
    do_lex(&c, s);

    return 0;
}
