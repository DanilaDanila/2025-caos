all: main.o sum.o
	gcc main.o sum.o -o a.out

main.o: main.c
	gcc -c main.c

sum.o: sum.s
	gcc -c sum.s

