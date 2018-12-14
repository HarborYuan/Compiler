CC=gcc
.c.o:
	echo rebuilding $*.o from $*.c
	$(CC) -ggdb -c $<

all: reg2dfa

reg2dfa: ast.o parser.o main.o 
	$(CC) -o ./reg2dfa $^

graph: ast.pdf

ast.pdf: ast.gv
	dot -Tpdf ast.gv -o ast.pdf

ast.o: ast.c ast.h set.h 
#dfa.o: dfa.c ast.h set.h
parser.o: parser.c ast.h set.h 
main.o: main.c ast.h set.h

clean: 
	rm -f reg2dfa ast.pdf ast.gv *.o 


