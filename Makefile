CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

refcount: $(OBJS) $(RT_OBJS)
	gcc -o refcount $(CFLAGS) $(OBJS)

clean:
	rm -f *.o refcount

