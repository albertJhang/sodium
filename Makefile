CFLAGS=-std=c11 -g -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

sodium: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): sodium.h

test: sodium
	./test.sh
	./test-driver.sh

clean:
	rm -f sodium *.o *~ tmp*

.PHONY: test clean