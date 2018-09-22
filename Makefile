# For building in different dir from source dir
ROOT = $(dir $(abspath $(firstword $(MAKEFILE_LIST))))

SRCS =  src/main.c \
		src/lex.c \
		src/parser.c \
		src/compile.c

CFLAGS = -Wall -g
EXE = sagiri

BIN_DIR = bin
OBJS = $(addprefix $(BIN_DIR)/, $(addsuffix .o, $(SRCS)))

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

$(BIN_DIR)/%.c.o: $(ROOT)/%.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CLAGS) -o $@ -c $<

clean:
	- rm $(OBJS)

.PHONY: clean all
