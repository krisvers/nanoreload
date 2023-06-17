CC = clang
CCFLAGS = -std=c99 -rdynamic

all:
	$(CC) main.c -o nanoreload $(CCFLAGS)
