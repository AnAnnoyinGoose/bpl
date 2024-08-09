CC=cc
CFLAGS=-Wall -Wextra -g
TARGET=bpl
SRC=./src/*
default:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)
