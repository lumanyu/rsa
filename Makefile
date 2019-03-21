TARGET = trsa
LIBS = -lm
CC = gcc

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

#HEADERS = huge.h
#OBJECTS = huge.o
CFLAGS=-Wall -O0 -g

#default: huge

%.o: %.c $(HEADERS)
	    $(CC) $(CFLAGS) -c $< -o $@

#huge: $(OBJECTS)
#	    gcc $(CFLAGS) $(OBJECTS) -o $@

#main: $(OBJECTS)
#	    gcc $(CFLAGS) $(OBJECTS) -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	    $(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	    -rm -f $(OBJECTS)
					#-rm -f program
					-rm -f $(TARGET)
