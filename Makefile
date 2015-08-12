CC=clang
CFLAGS=-c -Wall -std=c99 -MMD -MP
LDFLAGS=
SOURCES=main.c node.c selector.c
OBJECTS=$(SOURCES:.c=.o)
DEPENDENCIES=$(SOURCES:.c=.d)
EXECUTABLE=retain

.PHONY: all clean

all: $(EXECUTABLE)

-include $(DEPENDENCIES)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean :
	rm $(EXECUTABLE) $(OBJECTS) $(DEPENDENCIES)

.c.o:
	$(CC) $(CFLAGS) $< -o $@
