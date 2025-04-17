#-----------------------------------------------#
# Makefile for UNIX systems (macOS included)    #
# using a GNU C compiler (gcc)                  #
#-----------------------------------------------#

# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -ansi -Iinclude -I/opt/homebrew/opt/jpeg/include
LDFLAGS = -L/opt/homebrew/opt/jpeg/lib
LDLIBS = -ljpeg

# Directories
SRC = src
OBJ = obj
BIN = bin

# Object files
OBJECTS = $(OBJ)/main.o $(OBJ)/helpers.o

# Targets
all: $(BIN)/imgy

$(BIN)/imgy: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ)/main.o: $(SRC)/main.c include/helpers.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/helpers.o: $(SRC)/helpers.c include/helpers.h
	$(CC) $(CFLAGS) -c $< -o $@

# Phony targets
.PHONY: clean
clean:
	rm -f $(OBJ)/*.o $(OBJ)/*.os $(BIN)/imgy

# Ensure obj and bin directories exist
$(shell mkdir -p $(OBJ) $(BIN))
