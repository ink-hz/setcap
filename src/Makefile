CC=gcc
CFLAGS=-Wall -Werror -g
COMP_INC=-I../include
LDFLAGS=-lsetcap
SO_INC=-L../so/libsetcap -Wl,--rpath=../so/libsetcap

all: demo

demo:
	$(CC) $(CFLAGS) $(LDFLAGS) $(COMP_INC) $(SO_INC) main.c -o demo

clean:
	-rm -f *.o demo
