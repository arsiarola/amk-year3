GCC := $(shell command -v gcc 2>/dev/null)
ifdef GCC
echo "gcc path found: $(GCC)"
CC := gcc
else
echo "gcc not available, using cc"
CC := cc
endif

CFLAGS := -Wall -Wextra -Wfatal-errors -Wpedantic

SRCDIR := src
OBJDIR := obj
EXEDIR := .

SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
EXE := $(EXEDIR)/program

test:
	echo $(SRC)
	echo $(OBJ)

.PHONY: clean all

all : $(EXE)

$(EXE) : $(OBJ) | $(EXEDIR)
	    $(CC)  $^  -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	    $(CC) $(CFLAGS) -c $< -o $@

$(EXEDIR) $(OBJDIR):
	    mkdir -p $@

clean:
	@$(RM) -rv $(EXEDIR) $(OBJDIR)

