CFLAGS=-std=c11 -g -static -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

sodium:	$(OBJS)
		$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): sodium.h

test:	sodium
		./test.sh

clean:
		rm -f sodium *.o *~ tmp*

.PHONY: test clean