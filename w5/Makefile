# This Makefile sets the necessary compiler flags needed for the task.
# There is no need to change anything here.
FLAGS=-Wall -Werror -fpack-struct

all: bitmapfun

bitmapfun: bitmapfun.c
	gcc $(FLAGS) -o bitmapfun bitmapfun.c

test: bitmapfun
	./bitmapfun ./testpattern.bmp

clean:
	rm -f output.bmp
	rm -f bitmapfun
