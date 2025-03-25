#-----------------------------------------------#
# Makefile for UNIX systems (macOS included)    #
# using a GNU C compiler (gcc)                  #
#-----------------------------------------------#

# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -ansi  # Debug, warnings, ANSI C
LDFLAGS = -L/usr/local/lib  # Where libs live (Homebrew default on macOS)

# Directories
SRC = src
BIN = bin

# Targets
$(BIN)/imgy: $(SRC)/main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN)/imgy $(SRC)/main.c $(LDLIBS)

# Phony targets
.PHONY: clean
clean:
	rm -f $(BIN)/imgy

