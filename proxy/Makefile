CC = gcc
CFLAGS = -std=c17 -Wall -pthread -lrt

SRCS = main.c
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) main
