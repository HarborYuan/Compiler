cc = gcc
OBJ = lex.o main.o
MLIB = lex.h
TARGET = plain improved affix retval retinf

all: $(TARGET)

plain: $(OBJ) plain.o
	$(cc) -o plain $(OBJ) plain.o

improved: $(OBJ) improved.o
	$(cc) -o improved $(OBJ) improved.o

affix: $(OBJ) name.o affix.o
	$(cc) -o affix $(OBJ) name.o affix.o

retval: $(OBJ) name.o retval.o
	$(cc) -o retval $(OBJ) name.o retval.o

retinf: $(OBJ) name.o retinf.o
	$(cc) -o retinf $(OBJ) name.o retinf.o

%.o: %.c $(MLIB)
	$(cc) -c $< -o $@


clean:
	rm -rf *.o $(TARGET)

