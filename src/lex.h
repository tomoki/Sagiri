#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

// lex.c has keywords array, this is index of it.
enum keyword {
    KEY_AUTO,
    KEY_BREAK,
    KEY_CASE,
    KEY_CHAR,
    KEY_CONST,
    KEY_CONTINUE,
    KEY_DEFAULT,
    KEY_DO,
    KEY_DOUBLE,
    KEY_ELSE,
    KEY_ENUM,
    KEY_EXTERN,
    KEY_FLOAT,
    KEY_FOR,
    KEY_GOTO,
    KEY_IF,
    KEY_INLINE,
    KEY_INT,
    KEY_LONG,
    KEY_REGISTER,
    KEY_RESTRICT,
    KEY_RETURN,
    KEY_SHORT,
    KEY_SIGNED,
    KEY_SIZEOF,
    KEY_STATIC,
    KEY_STRUCT,
    KEY_SWITCH,
    KEY_TYPEDEF,
    KEY_UNION,
    KEY_UNSIGNED,
    KEY_VOID,
    KEY_VOLATILE,
    KEY_WHILE,
    KEY__BOOL,
    KEY__COMPLEX,
    KEY__IMAGINARD
};


// sorted by length
enum punctuator {
    PUNC_PERCENT_COLON_PERCENT_COLON,     // %:%:
    PUNC_RIGHT_SHIFT_EQUAL,               // >>=
    PUNC_LEFT_SHIFT_EQUAL,                // <<=
    PUNC_DOT_DOT_DOT,                     // ...
    PUNC_PERCENT_COLON,                   // %:
    PUNC_PERCENT_RIGHT_ANGLE,             // %>
    PUNC_LEFT_ANGLE_PERCENT,              // <%
    PUNC_COLON_RIGHT_ANGLE,               // :>
    PUNC_LEFT_ANGLE_COLON,                // <:
    PUNC_SHARP_SHARP,                     // ##
    PUNC_PIPE_EQUAL,                      // |=
    PUNC_CARET_EQUAL,                     // ^=
    PUNC_AND_EQUAL,                       // &=
    PUNC_MINUS_EQUAL,                     // -=
    PUNC_PLUS_EQUAL,                      // +=
    PUNC_PERCENT_EQUAL,                   // %=
    PUNC_SLASH_EQUAL,                     // /=
    PUNC_ASTERISK_EQUAL,                  // *=
    PUNC_PIPE_PIPE,                       // ||
    PUNC_AND_AND,                         // &&
    PUNC_BAN_EQUAL,                       // !=
    PUNC_EQUAL_EQUAL,                     // ==
    PUNC_RIGHT_ANGLE_EQUAL,               // >=
    PUNC_LEFT_ANGLE_EQUAL,                // <=
    PUNC_RIGHT_SHIFT,                     // >>
    PUNC_LEFT_SHIFT,                      // <<
    PUNC_MINUS_MINUS,                     // --
    PUNC_PLUS_PLUS,                       // ++
    PUNC_SHARP,                           // #
    PUNC_COMMA,                           // ,
    PUNC_EQUAL,                           // =
    PUNC_SEMICOLON,                       // ;
    PUNC_COLON,                           // :
    PUNC_QUESTION,                        // ?
    PUNC_PIPE,                            // |
    PUNC_CARET,                           // ^
    PUNC_RIGHT_ANGLE,                     // >
    PUNC_LEFT_ANGLE,                      // <
    PUNC_PERCENT,                         // %
    PUNC_SLASH,                           // /
    PUNC_BAN,                             // !
    PUNC_TILDE,                           // ~
    PUNC_MINUS,                           // -
    PUNC_PLUS,                            // +
    PUNC_ASTERISK,                        // *
    PUNC_AND,                             // &
    PUNC_DOT,                             // .
    PUNC_RIGHT_CURLY,                     // }
    PUNC_LEFT_CURLY,                      // {
    PUNC_RIGHT_PAREN,                     // )
    PUNC_LEFT_PAREN,                      // (
    PUNC_RIGHT_SQUARE,                    // ]
    PUNC_LEFT_SQUARE,                     // [
};

enum token_type {
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_KEYWORD,
    TOKEN_PUNCTUATOR
};

struct token {
    enum token_type type;
    union value_type {
        int integer;
        struct {
            char* start;
            int length;
        } identifier;
        enum keyword keyword;
        enum punctuator punctuator;
    } value;
};

struct state;
int lex(struct state*);

#endif
