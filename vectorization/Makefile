main: main.o
	gcc -o main main.o

main.o: main.c
	gcc -S -ftree-vectorizer-verbose=1 -Ofast --std=c99 -Wall -Wextra -c main.c -o main.S

.PHONY: clean

clean:
	rm -f main.o main
