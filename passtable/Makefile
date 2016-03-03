table: table.o
	gcc -o table table.o

table.o: table.c
	gcc -g -O0 --std=c99 -Wall -Wextra -c table.c -o table.o

.PHONY: clean

clean:
	rm -f table.o table
