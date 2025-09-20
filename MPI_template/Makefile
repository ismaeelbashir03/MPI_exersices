MF = Makefile

PROGRAM_NAME = main

# For Cirrus
#CC=	mpicc
#CFLAGS=	-cc=icc

# For ARCHER2
CC =	cc
CFLAGS =	-Iinclude

LFLAGS=	-lm

# Directories
BIN_DIR = bin
OUT_DIR = output

EXE = $(BIN_DIR)/$(PROGRAM_NAME)

INC =
DEPS := $(wildcard include/*.h)

SRC = ${PROGRAM_NAME}.c

OBJ	:= $(patsubst %.c,$(BIN_DIR)/%.o,$(SRC))

.PHONY: all clean

all: $(BIN_DIR) $(OUT_DIR) $(EXE)

$(BIN_DIR):
	mkdir -p $@

$(OUT_DIR):
	mkdir -p $@

$(BIN_DIR)/%.o: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LFLAGS)

$(OBJ): $(DEPS) $(MF)

clean:
	rm -rf $(BIN_DIR) core
