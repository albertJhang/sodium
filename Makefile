CFLAGS=-std=c11 -g -static

albertcc:	albertcc.c
test:	albertcc
		./test.sh
clean:
		rm -f albertcc *.o *~ tmp*

.PHONY: test clean