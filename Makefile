SRC = $(wildcard includes/*.c)

build: ${SRC}
	gcc ${SRC} $ -o server

debug: build/server ${SRC}
	${CC} ${SRC} $ -g -o server
	gdb ./build/server

run:
	./server