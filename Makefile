VPATH = src include
CFLAGS = -I include
CC = /usr/local/bin/gcc-7 -g3 -Wall -Wextra

main: partition.o main.o
	cc -g3 -Wall -Wextra main.o partition.o -o filter

clean:
	rm -rf *.o *.dSYM
