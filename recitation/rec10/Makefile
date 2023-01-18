# comments are always a great idea.

# we use this variable as $(CC) when building commands.
CC=gcc

# we keep using these flags in combination, so they're
# stuffed into a variable. Saves us from having to 
# re-type them repeatedly, and of course we can then
# change it in one place for all uses.
CFLAGS=-Og -std=c99

FUNC_OS=checkeq.o power.o countPos.o fib.o collatzLength.o 

# see below for the rec10 and switch targets.
all: rec10 switch

# the command 'make clean' should remove all generated files.
clean:
	-rm -rf *~ *# *.o rec10.s rec10.o rec10 switch.s switch.o switch

# target rec10 builds the overall executable, rec10.
# depends on all the individual functions' object files.
rec10: rec10.o  $(FUNC_OS)
	$(CC) $(CFLAGS) -o rec10 rec10.o $(FUNC_OS)

# compiles rec10.c into rec10.s; needs no other files.
rec10.s: rec10.c
	$(CC) $(CFLAGS) -S rec10.c

# compiles rec10.s into rec10.o; needs no other files.
rec10.o: rec10.s
	$(CC) $(CFLAGS) -c rec10.s

# each of the remaining targets also compiles each
# .s file into the corresponding .o file, with no
# other dependencies on other files.

checkeq.o: checkeq.s
	$(CC) $(CFLAGS) -c checkeq.s

power.o: power.s
	$(CC) $(CFLAGS) -c power.s

countPos.o: countPos.s
	$(CC) $(CFLAGS) -c countPos.s

fib.o: fib.s
	$(CC) $(CFLAGS) -c fib.s

collatzLength.o: collatzLength.s
	$(CC) $(CFLAGS) -c collatzLength.s


# target switch builds the switch executable.
switch: switch.o
	$(CC) $(CFLAGS) -o switch switch.o 

# compiles switch.c into switch.s; needs no other files.
switch.s: switch.c
	$(CC) $(CFLAGS) -S switch.c

# compiles switch.s into switch.o; needs no other files.
switch.o: switch.s
	$(CC) $(CFLAGS) -c switch.s





