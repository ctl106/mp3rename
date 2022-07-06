NAME =  mp3rename

SRCDIR = src
OBJDIR = obj
BINDIR = bin

DOXDIR = doxygen
DOXINDEX = $(DOXDIR)/html/index.html
DOXCONFIG = $(DOXDIR)/doxyfile


BINARY = $(BINDIR)/$(NAME)
HEADERS = $(wildcard $(SRCDIR)/*.h)
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

MKDIR = /usr/bin/mkdir
DOXYGEN = /usr/bin/doxygen

.PHONY: setup
setup:
	[ -d $(OBJDIR) ] || $(MKDIR) $(OBJDIR)
	[ -d $(BINDIR) ] || $(MKDIR) $(BINDIR)

.PHONY: all
all: setup
	$(MAKE) $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJECTS)
	$(MKDIR) $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINARY) $(OBJECTS)

$(BINARY):
	$(NAME)

.PHONY: clean
clean:
	$(RM) -f $(OBJECTS) $(BINARY) *~ *core

.PHONY: install
install:
	$(INSTALL) -c $(BINARY) $(PREFIX)/bin/$(NAME)
	$(INSTALL) -c $(BINARY).1.gz $(PREFIX)/man/man1/
    
.PHONY: docs
docs: $(SRCDIR)
	export OUTPUT_DIRECTORY=$(DOXDIR); $(DOXYGEN) $(DOXCONFIG)