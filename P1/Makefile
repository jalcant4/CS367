#-----------------------------------------------------------------------------
# Makefile
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Choose a compiler and its options
#--------------------------------------------------------------------------
CC   = gcc -std=gnu99	# Use gcc for Zeus
OPTS = -Og -Wall -Werror -Wno-error=unused-variable -Wno-error=unused-function -pthread
DEBUG = -g					# -g for GDB debugging

#--------------------------------------------------------------------
# Build Environment
#--------------------------------------------------------------------
SRCDIR=./src
OBJDIR=./obj
INCDIR=./inc
BINDIR=.

#--------------------------------------------------------------------
# Build Files
#--------------------------------------------------------------------
SRCS=$(wildcard $(SRCDIR)/vm*.c)
HDRS=$(wildcard $(INCDOR)/*.h)

#--------------------------------------------------------------------
# Compiler Options
#--------------------------------------------------------------------
INCLUDE=$(addprefix -I,$(INCDIR))
LIBRARY=$(addprefix -L,$(OBJDIR))
SRCOBJS=${SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o}
OBJS=$(OBJDIR)/vm.o $(OBJDIR)/vm_cs.o $(OBJDIR)/vm_shell.o $(OBJDIR)/avan_sched.o $(OBJDIR)/vm_support.o
CFLAGS=$(OPTS) $(INCLUDE) $(LIBRARY) $(DEBUG)

#--------------------------------------------------------------------
# Build Recipies for the Executables (binary)
#--------------------------------------------------------------------
TARGET = $(BINDIR)/vm 

all: $(TARGET) 

tester: $(TARGET) $(SRCDIR)/test_avan_sched.c $(OBJDIR)/vm_support.o $(OBJDIR)/avan_sched.o
	${CC} $(CFLAGS) -o $@ $(SRCDIR)/test_avan_sched.c $(OBJDIR)/vm_support.o $(OBJDIR)/avan_sched.o

helpers:
	make -f Makefile.helpers

# Links the object files to create the target binary
$(TARGET): $(OBJS) $(HDRS) $(INCDIR)
	${CC} ${CFLAGS} -o $@ $(OBJS) -lvm_sd

#$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c 
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/vm_settings.h					 
	${CC} $(CFLAGS) -c -o $@ $<

#--------------------------------------------------------------------
# Cleans the binaries
#--------------------------------------------------------------------
clean:
	rm -f $(SRCOBJS) $(TARGET) $(HELPER_TARGETS)

cleanall:
	rm -f $(SRCOBJS) $(TARGET) $(HELPER_TARGETS)
	make -f Makefile.helpers clean
