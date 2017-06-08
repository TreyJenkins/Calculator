all: clean
	gcc-6 main.c -std=c99
clean:
	- rm a.out
