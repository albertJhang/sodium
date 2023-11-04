CFLAGS=-std=c11 -g -static

sodium:	sodium.c
test:	sodium
		./test.sh
clean:
		rm -f sodium *.o *~ tmp*

.PHONY: test clean