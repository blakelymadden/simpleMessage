CLFAGS=-Wall -g

default: all

all: 
	gcc -Wall -g -o simpleMessage simple-message-s.c

clean:
	rm -f *~ *.o *.gch *.out simpleMessage
