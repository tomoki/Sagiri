# For building in different dir from source dir
ROOT = $(dir $(abspath $(firstword $(MAKEFILE_LIST))))

SRCS =  src/main.c \
		src/lex.c \
		src/parse.c \
		src/compile.c \
		src/vector.c \
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
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/variable-in-function-use.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/var2.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/var3.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/var4.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/var5.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/shadowing-1.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/shadowing-2.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/shadowing-3.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/two-functions-just-def.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/two-functions-use.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/two-functions-use-plus.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/two-functions-with-argument1.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/two-functions-with-argument2.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/call-exit.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/call-putc.c $(ROOT)/test/startup.c
	@ $(ROOT)/test/test.sh $(abspath $(EXE)) $(ROOT)/test/case/my-main.c $(ROOT)/test/no-startup.c


.PHONY: clean test
