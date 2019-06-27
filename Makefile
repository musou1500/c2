CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

refcount: $(OBJS) $(RT_OBJS)
	gcc -o refcount $(CFLAGS) $(OBJS)

clean:
	rm -f *.o refcount

format:
	@for file in $(SRCS) $(HEADERS); do \
		clang-format-6.0 -i "$$file"; \
	done
