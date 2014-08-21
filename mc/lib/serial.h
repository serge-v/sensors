#include <stdio.h>

int serial_putchar(char c, FILE *stream);
int serial_getchar(FILE *stream);
void serial_init(void);
uint8_t serial_available(void);

