CC=gcc
CFLAGS=-Wall -Wextra -Wformat -pedantic -ggdb
IDIR=include
INCLUDE=-I$(IDIR)/
LIBS=
SRCS=main.c
OUT=a.out

build:
	$(CC) $(CFLAGS) $(INCLUDE) -o $(OUT) $(SRCS) $(LIBS)

run: build
	./$(OUT)
