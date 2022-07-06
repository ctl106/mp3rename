NAME =  mp3rename

SRCDIR = src
OBJDIR = obj
BINDIR = bin

BINARY = $(BINDIR)/$(NAME)
HEADERS = $(wildcard $(SRCDIR)/*.h)
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

MKDIR = /usr/bin/mkdir

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

.PHONU: install
install:
	$(INSTALL) -c $(BINARY) $(PREFIX)/bin/$(NAME)
	$(INSTALL) -c $(BINARY).1.gz $(PREFIX)/man/man1/
    