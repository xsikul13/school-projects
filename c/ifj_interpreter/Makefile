CFLAGS = -std=c99 -Wall -Wextra -pedantic -g

$(CC) = gcc

all: projekt

debug: CFLAGS += -DDEBUG
debug: projekt

statistics: CFLAGS += -DSTATISTICS
statistics: projekt

interpret.o: interpret.c interpret.h scaner.h str.h ilist.h errors.h ial.h sa_pa.h
	$(CC) $(CFLAGS) -c interpret.c -o interpret.o

errors.o: errors.c errors.h
	$(CC) $(CFLAGS) -c errors.c -o errors.o

ial.o: ial.c ial.h errors.h
	$(CC) $(CFLAGS) -c ial.c -o ial.o

ilist.o: ilist.c ilist.h errors.h
	$(CC) $(CFLAGS) -c ilist.c -o ilist.o

scaner.o: scaner.c scaner.h str.h errors.h
	$(CC) $(CFLAGS) -c scaner.c -o scaner.o

str.o: str.c str.h
	$(CC) $(CFLAGS) -c str.c -o str.o

sa_pa.o: sa_pa.c sa_pa.h scaner.h ial.h ilist.h
	$(CC) $(CFLAGS) -c sa_pa.c -o sa_pa.o

parser.o: parser.c parser.h scaner.h str.h ial.h errors.h ilist.h sa_pa.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

main.o: main.c parser.h scaner.h str.h ial.h errors.h ilist.h sa_pa.h interpret.h
	$(CC) $(CFLAGS) -c main.c -o main.o


projekt: main.o parser.o scaner.o str.o ial.o errors.o ilist.o sa_pa.o interpret.o
	$(CC) $(CFLAGS) main.o interpret.o parser.o scaner.o str.o ial.o errors.o ilist.o sa_pa.o -o projekt

clean: 
	rm -f *.o
	rm -f projekt
	rm -f diff.out
	rm -f Test.out


