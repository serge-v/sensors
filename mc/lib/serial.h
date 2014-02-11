#include <stdio.h>

void serial_putchar(char c, FILE *stream);
char serial_getchar(FILE *stream);
void serial_init(void);
uint8_t serial_available(void);

