CC := gcc
CFLAGS := -Wall -Iinclude

# Define the source files and corresponding object files
SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, build/%.o, $(SRCS))

# Define the target executable
TARGET := bin/main

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to build object files
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf bin/*
	rm -rf build/*