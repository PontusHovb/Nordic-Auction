CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lSDL2 -lSDL2_ttf
TARGET = order_book
SRC = order_book.c GUI.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
