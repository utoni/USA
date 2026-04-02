all: usa

CC = g++

usa-dbg:
	$(CC) -Wall -O0 -g3 -fstrict-aliasing -fsanitize=address -fsanitize=leak -fsanitize=undefined -DSTB_IMAGE_IMPLEMENTATION=1 *.c *.cpp -o usa-dbg -lglfw

usa-val:
	$(CC) -Wall -O0 -g3 -DSTB_IMAGE_IMPLEMENTATION=1 *.c *.cpp -o usa-val -lglfw

usa:
	$(CC) -Wall -O3 -fomit-frame-pointer -DSTB_IMAGE_IMPLEMENTATION=1 *.c *.cpp -o usa -lglfw

run-dbg: usa-dbg
	./usa-dbg

run-val: usa-val
	valgrind ./usa-val

run: usa
	./usa

.PHONY: all usa-dbg usa-val usa
