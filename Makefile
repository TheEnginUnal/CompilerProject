# Compiler and tools
CC = gcc
FLEX = flex
CFLAGS = -Wall

# The default target
all: scanner

# How to build the executable
scanner: lex.yy.c
	$(CC) $(CFLAGS) lex.yy.c -o scanner

# How to generate the C file from Flex
lex.yy.c: scanner.l
	$(FLEX) scanner.l

# Clean up generated files
clean:
	rm -f scanner lex.yy.c