CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -Wall

all: bminor

# ast.c is now included in the compilation!
bminor: parser.tab.c lex.yy.c ast.c symbol.c scope.c resolve.c typecheck.c ir.c codegen.c
	$(CC) $(CFLAGS) parser.tab.c lex.yy.c ast.c symbol.c scope.c resolve.c typecheck.c ir.c codegen.c -o bminor

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

lex.yy.c: scanner.l parser.tab.h
	$(FLEX) scanner.l

clean:
	rm -f bminor lex.yy.c parser.tab.c parser.tab.h