CFLAGS=-std=c11 -g -fno-common

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TEST_SRCS=$(wildcard test/*.c)
TESTS=$(TEST_SRCS:.c=.out)

sodium: $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): sodium.h

test/%.out: sodium test/%.c
		$(CC) -o- -E -P -C test/$*.c | ./sodium -o test/$*.s -
		$(CC) -o $@ test/$*.s -xc test/common

test: $(TESTS)
		for i in $^; do echo $$i; ./$$i || exit 1; echo; done
		test/driver.sh

clean:
		rm -rf sodium tmp* $(TESTS) test/*.s test/*.out
		find * -type f '(' -name '*~' -o -name '*.o' ')' -exec rm {} ';'

.PHONY: test clean