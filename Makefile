# Compiler
CC = gcc

# Source files
SRCS = main.c uno_game.c

# Header files
HEADERS = uno_game.h

# Object files
OBJS = $(SRCS:.c=.o)

# Output executable
TARGET = uno_game

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
