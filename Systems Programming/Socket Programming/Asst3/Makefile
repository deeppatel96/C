PROGRAMS = libnetfiles netfileserver

CC = gcc
CFLAGS = -Wall -pthread

%: %.c %.h 
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: all clean

all: $(PROGRAMS)

clean:
	@rm -f *.o $(PROGRAMS)