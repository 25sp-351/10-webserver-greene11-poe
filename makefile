CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
SRCS = main.c connection_handler.c calc_handler.c router.c request.c response.c
HDRS = connection_handler.h calc_handler.h router.h request.h response.h
OBJS = $(SRCS:.c=.o)
TARGET = webserver

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lpthread
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)
.PHONY: all clean
all: $(TARGET)
