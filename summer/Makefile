# Project: SUMMER LANGUAGE

CPP  = g++
CC   = gcc
OBJ  = obj/asm.o obj/core.o obj/lex.o obj/summer.o
BIN  = summer
CFLAGS = -O2 -Wall
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before summer all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o "summer" $(LIBS)

obj/asm.o: src/asm.c
	$(CC) -c src/asm.c -o obj/asm.o $(CFLAGS)

obj/core.o: src/core.c
	$(CC) -c src/core.c -o obj/core.o $(CFLAGS)

obj/lex.o: src/lex.c
	$(CC) -c src/lex.c -o obj/lex.o $(CFLAGS)

obj/summer.o: src/summer.c
	$(CC) -c src/summer.c -o obj/summer.o $(CFLAGS)
