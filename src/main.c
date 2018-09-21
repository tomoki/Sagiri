#include "compile.h"
#include "state.h"
#include "lex.h"
#include "parser.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    struct state s;
    int error_code = 0;
    if ((error_code = lex(&s)) != 0) {
        fprintf(stderr, "Failed to lex (%d)\n", error_code);
        return 1;
    }

    if ((error_code = parse(&s) != 0)) {
        fprintf(stderr, "Failed to parse (%d)\n", error_code);
        return 1;
    }

    if ((error_code = compile(&s) != 0)) {
        fprintf(stderr, "Failed to compile (%d)\n", error_code);
        return 1;
    }

    return 0;
}
