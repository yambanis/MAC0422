SRCDIR	= src
EXTDIR	= ext
HDRDIR  = headers
OBJDIR	= obj
BINDIR	= bin
DEPDIR	= dep
DATDIR	= data

CC	= clang
MAC	= mac422shell
CFLAGS	= -std=c99 -Ofast -msse2 -march=native -Wall -Wextra -Wpedantic -I$(HDRDIR)
LFLAGS	=

SOURCES	:= $(wildcard $(SRCDIR)/*.c)
EXTERNS	:= $(wildcard $(EXTDIR)/*.c)
HEADERS := $(wildcard $(HDRDIR)/*.h)
OBJECTS	:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
EXT_OBJ	:= $(EXTERNS:$(EXTDIR)/%.c=$(OBJDIR)/%.o)
DEPS	:= $(OBJECTS:$(OBJDIR)/%.o=$(DEPDIR)/%.d)

REMOVE	:= rm -rf

.PHONY: all
all: $(MAC)

# Linking
$(MAC): $(OBJECTS) $(EXT_OBJ) $(CMN_OBJ)
		$(CC) $(LFLAGS) mac422shell.c -o $@ $(OBJECTS) $(EXT_OBJ) $(CMN_OBJ)
		@echo "linking complete"

-include $(DEPS)

# Compilation
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
		mkdir -p $(OBJDIR)
		mkdir -p $(DEPDIR)
		$(CC) -c $(CFLAGS) $< -o $@
		$(CC) -I$(HDRDIR) -MM -MT '$(OBJDIR)/$*.o' $(SRCDIR)/$*.c > $(DEPDIR)/$*.d
		@echo "Compiled $<"

$(EXT_OBJ): $(OBJDIR)/%.o : $(EXTDIR)/%.c
		mkdir -p $(OBJDIR)
		mkdir -p $(DEPDIR)
		$(CC) -c $(CFLAGS) $< -o $@
		$(CC) -I$(HDRDIR) -MM -MT '$(OBJDIR)/$*.o' $(EXTDIR)/$*.c > $(DEPDIR)/$*.d
		@echo "Compiled $<"

.PHONY: clean
clean:
#$(REMOVE) $(OBJECTS) $(OBJDIR) $(BINDIR) $(DEPDIR)
		$(REMOVE) $(OBJECTS) $(OBJDIR) $(BINDIR) $(DEPDIR) $(MAC) *.maco
		@echo "Deleted $<"


