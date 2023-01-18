# comments are always a great idea.

# we use this variable as $(CC) when building commands.
CC=gcc

# we keep using these flags in combination, so they're
# stuffed into a variable. Saves us from having to 
# re-type them repeatedly, and of course we can then
# change it in one place for all uses.
CFLAGS=-O0 -std=c99

FUNC_OS=dot4.o call_dot4.o scale.o call_scale.o call_sum.o intermediate.o rec_fib.o 

# see below for the rec11 target.
all: rec11

# the command 'make clean' should remove all generated files.
clean:
	-rm -rf *~ *# $(FUNC_OS) rec11.s rec11.o rec11

# target rec11 builds the overall executable, rec11.
# depends on all the individual functions' object files.
rec11: rec11.o  $(FUNC_OS) support_funcs.o
	$(CC) $(CFLAGS) -o rec11 rec11.o $(FUNC_OS) support_funcs.o

# compiles rec11.c into rec11.s; needs no other files.
rec11.s: rec11.c
	$(CC) $(CFLAGS) -S rec11.c

# compiles rec11.s into rec11.o; needs no other files.
rec11.o: rec11.s
	$(CC) $(CFLAGS) -c rec11.s

# each of the remaining targets also compiles each
# .s file into the corresponding .o file, with no
# other dependencies on other files.

call_dot4.o: call_dot4.s
	$(CC) $(CFLAGS) -c call_dot4.s

dot4.o: dot4.s
	$(CC) $(CFLAGS) -c dot4.s

scale.o: scale.s
	$(CC) $(CFLAGS) -c scale.s

call_scale.o: call_scale.s
	$(CC) $(CFLAGS) -c call_scale.s

call_sum.o: call_sum.s
	$(CC) $(CFLAGS) -c call_sum.s

intermediate.o: intermediate.s
	$(CC) $(CFLAGS) -c intermediate.s

rec_fib.o: rec_fib.s
	$(CC) $(CFLAGS) -c rec_fib.s
