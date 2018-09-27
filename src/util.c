#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void error(char* error_message)
{
    fprintf(stderr, "%s\n", error_message);
    __asm__("int3");
}
