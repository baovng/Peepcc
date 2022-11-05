#######
# Makefile for peep compiler
#######

CC = gcc 
CFLAGS = -O0 -g -Wall -Werror

all: peepcc test-i2p

peepcc: peepcc.o infToPstf.o 
	$(CC) $(CFLAGS) -o peepcc peepcc.o infToPstf.o

peepcc.o : peepcc.h infToPstf.h
	$(CC) $(CFLAGS) -c peepcc.h infToPstf.h 

infToPstf.o : infToPstf.h
	$(CC) $(CFLAGS) -c infToPstf.h 

test-i2p:
	echo "This is the test-i2p target"

clean:
	rm -f *.o *~ peepcc 
	rm -rf *.dSYM
	(cd traces; rm -f *~)