#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

void error(char* error_message)
{
    fprintf(stderr, "%s\n", error_message);
    exit(1);
}

#endif
