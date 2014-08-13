#include <stdio.h>

int main()
{
	unsigned long a = 0xFFFFFFF0UL;
	unsigned long b = 15;
	
	printf("a: 0x8x, b: %0x8x\n", a, b);
	printf("ulong: a < b : %d\n", a < b);
	printf("ulong: a < b : %d\n", (b - a) > 0);

	long c = 0xFFFFFFF0L;
	long d = 15;

	printf("c: 0x8x, d: %0x8x\n", c, d);
	printf("long: c < d : %d\n", c < d);
	printf("long: c < d : %d\n", (d - c) > 0);
}
