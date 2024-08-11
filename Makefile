CC=cc
CFLAGS=-Wall -Wextra
TARGET=build/bpl
SRC=./src/*
DFLAGS=-DDEBUG -g
debug:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(DFLAGS)

release:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

clean:
	rm -f $(TARGET)
