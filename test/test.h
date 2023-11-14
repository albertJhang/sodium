#define ASSERT(x, y) assert(x, y, #y)

void assert(int expect, int actual, char *code);
int printf();