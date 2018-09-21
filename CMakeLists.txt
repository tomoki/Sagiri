cmake_minimum_required (VERSION 3.9)

project (sagiri)

set (source
        src/main.c
        src/lex.c
        src/parser.c
        src/compile.c)

add_executable(sagiri ${source})
