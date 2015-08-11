CC=clang
CFLAGS=-c -Wall -std=c99
LDFLAGS=
SOURCES=main.c node.c selector.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=retain

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean :
	rm $(EXECUTABLE) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@
