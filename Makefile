SRC = $(wildcard includes/*.c)

build: ${SRC}
	gcc ${SRC} $ -o server

debug: ${SRC}
	${CC} ${SRC} $ -g -o server
	gdb ./server

run:
	./server