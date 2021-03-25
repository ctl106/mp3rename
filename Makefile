PROG =  mp3rename
SRCS =  mp3rename.c 
OBJS =  mp3rename.o 
RM = /bin/rm

all: mp3rename

mp3rename: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) 

clean:
	$(RM) -f $(OBJS) $(PROG) *~ *core

install:
	$(INSTALL) -c mp3rename $(PREFIX)/bin/mp3rename
	$(INSTALL) -c mp3rename.1.gz $(PREFIX)/man/man1/
    