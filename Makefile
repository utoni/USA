all: usa

CC = g++

usa-dbg:
	$(CC) -Wall -O0 -g3 -fstrict-aliasing -fsanitize=address -fsanitize=leak -fsanitize=undefined -DSTB_IMAGE_IMPLEMENTATION=1 *.c *.cpp -o usa-dbg -lglfw

usa:
	$(CC) -Wall -O3 -fomit-frame-pointer -DSTB_IMAGE_IMPLEMENTATION=1 *.c *.cpp -o usa -lglfw

run-dbg: usa-dbg
	./usa-dbg

run: usa
	./usa

.PHONY: all usa-dbg usa
