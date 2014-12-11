CC=gcc
CFLAGS=-O2 -march=native
LDFLAGS=-lm -lalleg

SOURCE=$(wildcard *.c)
DEPENDENCY=$(wildcard *.h) Makefile

all: transdimensional_hellspider

transdimensional_hellspider: $(SOURCE) $(DEPENDENCY)
	$(CC) $(SOURCE) $(CFLAGS) $(LDFLAGS) -o transdimensional_hellspider
