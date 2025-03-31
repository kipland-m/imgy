#-----------------------------------------------#
# Makefile for UNIX systems (macOS included)    #
# using a GNU C compiler (gcc)                  #
#-----------------------------------------------#

# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -I/opt/homebrew/opt/jpeg/include  
LDFLAGS = -L/opt/homebrew/opt/jpeg/lib  
LDLIBS = -ljpeg  

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
