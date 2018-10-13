# For building in different dir from source dir
ROOT = $(dir $(abspath $(firstword $(MAKEFILE_LIST))))

SRCS =  src/main.c \
		src/lex.c \
		src/parser.c \
		src/compile.c \
		src/util.c \

CFLAGS = -Wall -g
EXE = sagiri

BIN_DIR = bin
OBJS = $(addprefix $(BIN_DIR)/, $(addsuffix .o, $(SRCS)))

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)

$(BIN_DIR)/%.c.o: $(ROOT)/%.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	- rm $(OBJS)

test: $(EXE)
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/just-return.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/add1.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/add2.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/minus1.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/minus2.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/exp-paren.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/two-return.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/if-true.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/if-false.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/if-just-true.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/if-just-false.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/multi-if1.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/multi-if2.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/multi-if3.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/multi-if4.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/variable-in-function.c $(ROOT)/test/startup.c

.PHONY: clean test