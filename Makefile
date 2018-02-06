VPATH = src include
CFLAGS = -I include
CC = gcc -g3 -Wall -Wextra

main: main.o
	gcc -g3 -Wall -Wextra main.o -o filter

clean:
	rm -rf *.o *.dSYM
