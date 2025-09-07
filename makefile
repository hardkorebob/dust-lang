# Makefile for the Dust Compiler

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g
TARGET = dustc

# NEW: Added utils.c to the list of source files
SRCS = main.c lexer.c parser.c codegen.c component_system.c type_table.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
