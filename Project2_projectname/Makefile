CC = gcc
CFLAGS = -Iinclude -lpthread
SRCS = src/main.c src/server.c src/config.c src/logger.c src/http.c src/authentication.c
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))
TARGET = web_server

all: build $(TARGET)

# Create the build directory if it doesn’t exist
build:
	mkdir -p build

# Link all object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# Compile .c files to .o files and place them in the build directory
build/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up build artifacts
clean:
	rm -f build/*.o $(TARGET)
