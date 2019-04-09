# Project: SUMMER LANGUAGE

CPP  = g++
CC   = gcc
OBJ  = obj/core.o obj/lex.o obj/asm.o obj/disasm.o obj/assemble.o obj/summer.o
BIN  = summer
CFLAGS = -O2 -Wall
ifeq ($(OS),Windows_NT)
  # DirectX
  #LIBS = -ld3dx8d -ld3d8 -lgdi32 -lwinmm
	LIBS =
#  RES  = obj/resource.o
  RES =
else
  LIBS = -ldl
  RES =
endif
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before summer all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o "summer" $(LIBS) $(RES)

obj/core.o: src/core.c
	$(CC) $(CFLAGS) -c src/core.c -o obj/core.o

obj/lex.o: src/lex.c
	$(CC) $(CFLAGS) -c src/lex.c -o obj/lex.o

obj/asm.o: src/asm.c
	$(CC) $(CFLAGS) -c src/asm.c -o obj/asm.o

obj/disasm.o: src/disasm.c
	$(CC) $(CFLAGS) -c src/disasm.c -o obj/disasm.o

obj/assemble.o: src/assemble.c
	$(CC) $(CFLAGS) -c src/assemble.c -o obj/assemble.o

obj/summer.o: src/summer.c
	$(CC) $(CFLAGS) -c src/summer.c -o obj/summer.o

