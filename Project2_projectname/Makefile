CC = gcc
CFLAGS = -Iinclude -lpthread
SRCS = src/main.c src/server.c src/config.c src/logger.c src/http.c src/authentication.c
OBJS = $(SRCS:.c=.o)
TARGET = web_server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
