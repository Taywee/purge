CC=gcc
DESTDIR=/usr
CFLAGS=-c -ansi -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -std=c99 -MMD -MP -O2
LDFLAGS=-s 
SOURCES=main.c node.c selector.c
OBJECTS=$(SOURCES:.c=.o)
DEPENDENCIES=$(SOURCES:.c=.d)
EXECUTABLE=retain

.PHONY: all clean install uninstall

all: $(EXECUTABLE)

-include $(DEPENDENCIES)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

install: $(EXECUTABLE)
	install -d $(DESTDIR)/bin
	install -d $(DESTDIR)/share/man/man1
	install $(EXECUTABLE) $(DESTDIR)/bin/
	gzip -c doc/retain.1 > $(DESTDIR)/share/man/man1/retain.1.gz

uninstall: $(EXECUTABLE)
	-rm $(DESTDIR)/bin/$(EXECUTABLE) $(DESTDIR)/share/man/man1/retain.1.gz

clean :
	rm $(EXECUTABLE) $(OBJECTS) $(DEPENDENCIES)

.c.o:
	$(CC) $(CFLAGS) $< -o $@
