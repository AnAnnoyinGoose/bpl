CC=cc
CFLAGS=-Wall -Wextra -g
TARGET=build/bpl
SRC=./src/*
default:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)
clean:
	rm -f $(TARGET)
