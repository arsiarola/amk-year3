# we test to see if a command has a path via "command -v" and redirect errors to dev/null
# the actual output of the command will be assigned to variable GCC
# If no path is found nothing will be returned meaning our variable GCC will not be defined,
# so we can do easy ifdef check and set our CC variable depending on the case

destPath ?= ~/
destIP   ?= 192.168.0.17
destUser ?= arsia

ifdef compiler
CC := $(compiler)
else
GCC := $(shell command -v gcc 2>/dev/null)
ifdef GCC
echo "gcc path found: $(GCC)"
CC := gcc
else
echo "gcc not available, using cc"
CC := cc
endif # GCC
endif # compiler

# General variables for flags and directories
CFLAGS := -Wall -Wextra -Wfatal-errors -Wpedantic -std=c99
SRCDIR := ./src
OBJDIR := ./obj
EXEDIR := .

# Variables for different files:
SRC := $(wildcard $(SRCDIR)/*.c)          # source files (.c)
OBJ := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o) # object files (.o)
DEP := $(OBJ:.o=.d)                       # dependency files (.d), contains information
                                          # of which #includes the c files contain
EXE := $(EXEDIR)/a.out                    # executable

all : $(EXE)

# pipe symbol means exedir will be created before any object files are built
# and also the timestamp of the exedir doesn't affect objcet files being rebuilt
# $^ means Put everything that the "prequsites" contain
# $@ means Put all the "targets"
$(EXE) : $(OBJ) | $(EXEDIR)
	    $(CC)  $^ -o $@

# Use also flag -MD to generate .d files, so we can have recompilation of a file
# even if only the header file is changed

$(OBJDIR)/%.o : $(SRCDIR)/%.c  | $(OBJDIR)
	    $(CC) $(CFLAGS) -MD -MP -c $< -o $@

$(EXEDIR) $(OBJDIR) :
	    mkdir -p $@

-include $(DEP)

scp : $(EXE)
	scp $(EXE) $(destUser)@$(destIP):$(destPath)


# first remove all the wanted files, then try to remove that directory,
# so if the directory contains something extra, then the directory and that extra
# content will not be removed.
clean:
	@$(RM) $(EXE) $(OBJ) $(DEP)
	@$(RM) -d $(OBJDIR)

# Make doesn't think clean and all are files, but rather just commands.
.PHONY: clean all scp

