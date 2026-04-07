CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -Wall

# Derleyicimizin yeni adı "bminor" olsun
all: bminor

bminor: parser.tab.c lex.yy.c
	$(CC) $(CFLAGS) parser.tab.c lex.yy.c -o bminor

# Bison, parser.y dosyasından .c ve .h dosyalarını üretir
parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

# Flex, parser.tab.h'a ihtiyaç duyar
lex.yy.c: scanner.l parser.tab.h
	$(FLEX) scanner.l

clean:
	rm -f bminor lex.yy.c parser.tab.c parser.tab.h