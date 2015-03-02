#include <stdio.h>
#include <stdint.h>

static void print_bin(uint8_t c, uint8_t prev)
{
	uint8_t r = 0x80;
	
	while (r)
	{
		if (c & r)
			printf("1");
		else
			printf("0");
		r >>= 1;
	}
	printf(" ");

	r = 0x80;
	
	while (r)
	{
		if (c & r)
		{
			if (prev & r)
			{
				printf("  \x1b[31m\xe2\x94\x82\x1b[0m");
			}
			else
			{
				printf(" \xe2\x94\x94\x1b[31m\xe2\x94\x90\x1b[0m");
			}
		}
		else
		{
			if (prev & r)
			{
				printf(" \xe2\x94\x8c\x1b[31m\xe2\x94\x98\x1b[0m");
			}
			else
			{
				printf(" \xe2\x94\x82 ");
			}
		}
		r >>= 1;
	}
	printf("\n");
}

int main()
{
	printf("                        SCK SO SI SS  9 NSEL\n");
	printf("                         13 12 11 10  9  8\n");
	printf("                    7  6  5  4  3  2  1  0\n");
	
	if (1)
	{
		print_bin(0x00, 0x00);
		print_bin(0x00, 0x00);
		print_bin(0xAA, 0x00);
		print_bin(0xAA, 0xAA);
		print_bin(0xAA, 0xAA);
		print_bin(0x00, 0xAA);
		print_bin(0x00, 0x00);
		return 0;
	}

	int n = 0, prev_n = 0;
	int skip = 0;
	int skip_count = 0;
	uint8_t buf[4096];
	FILE* f = fopen("/dev/stdin", "rb");
	uint8_t prev = 0;
	
	while (!feof(f))
	{
		int was_read = fread(buf, 1, 4096, f);
		int i = 0;
		for (i = 0; i < was_read; i++, n++)
		{
			if (buf[i] == prev)
			{
				skip_count++;
			}
			else
			{
				skip = 0;
				skip_count = 0;
				
				if (n - prev_n > 1000)
				{
					printf("-\n");
				}
			}

			if (!skip && skip_count > 1)
			{
				skip = 1;
//				printf("-\n");
				prev_n = n;
				continue;
			}
			
			if (skip)
				continue;
			
			printf("%8d  ", n);
			print_bin(buf[i], prev);
			prev = buf[i];
		}
	}
	printf("%8d\n", n);
}

