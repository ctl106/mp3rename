NAME =  mp3rename

SRCDIR = src
OBJDIR = obj
BINDIR = bin

BINARY = $(BINDIR)/$(NAME)
HEADERS = $(wildcard $(SRCDIR)/*.h)
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
#RM = /bin/rm

.PHONY: all clean install

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BINARY) $(OBJECTS)

$(BINARY):
	$(NAME)

clean:
	$(RM) -f $(OBJECTS) $(BINARY) *~ *core

install:
	$(INSTALL) -c $(BINARY) $(PREFIX)/bin/$(NAME)
	$(INSTALL) -c $(BINARY).1.gz $(PREFIX)/man/man1/
    