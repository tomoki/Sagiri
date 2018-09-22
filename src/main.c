#include "compile.h"
#include "state.h"
#include "lex.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    struct state s;
    int error_code = 0;
    FILE *f;

    if ((f = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Failed to open %s\n", argv[1]);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    int fsize = ftell(f);
    rewind(f);
    // for NULL, +1
    s.source = malloc(fsize + sizeof(char));
    s.source_len = fsize;
    fread(s.source, fsize, 1, f);

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
