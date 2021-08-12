CC=gcc
CFLAGS=-c -Wall -Werror -fPIC -g
COMP_INC=-I../../include
LDFLAGS=-shared -lcap

all: libsetcap.o libsetcap.so

libsetcap.o:
	$(CC) $(COMP_INC) $(CFLAGS) libsetcap.c

libsetcap.so:
	$(CC) $(LDFLAGS) libsetcap.o -o libsetcap.so

clean:
	-rm -f *.o *.so
