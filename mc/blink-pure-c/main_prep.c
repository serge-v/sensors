# 1 "main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "main.c"
# 1 "/opt/avr-gcc/avr/include/avr/interrupt.h" 1 3
# 38 "/opt/avr-gcc/avr/include/avr/interrupt.h" 3
# 1 "/opt/avr-gcc/avr/include/avr/io.h" 1 3
# 99 "/opt/avr-gcc/avr/include/avr/io.h" 3
# 1 "/opt/avr-gcc/avr/include/avr/sfr_defs.h" 1 3
# 126 "/opt/avr-gcc/avr/include/avr/sfr_defs.h" 3
# 1 "/opt/avr-gcc/avr/include/inttypes.h" 1 3
# 37 "/opt/avr-gcc/avr/include/inttypes.h" 3
# 1 "/opt/avr-gcc/lib/gcc/avr/4.9.2/include/stdint.h" 1 3 4
# 9 "/opt/avr-gcc/lib/gcc/avr/4.9.2/include/stdint.h" 3 4
# 1 "/opt/avr-gcc/avr/include/stdint.h" 1 3 4
# 122 "/opt/avr-gcc/avr/include/stdint.h" 3 4
typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef signed int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
# 143 "/opt/avr-gcc/avr/include/stdint.h" 3 4
typedef int16_t intptr_t;




typedef uint16_t uintptr_t;
# 160 "/opt/avr-gcc/avr/include/stdint.h" 3 4
typedef int8_t int_least8_t;




typedef uint8_t uint_least8_t;




typedef int16_t int_least16_t;




typedef uint16_t uint_least16_t;




typedef int32_t int_least32_t;




typedef uint32_t uint_least32_t;







typedef int64_t int_least64_t;






typedef uint64_t uint_least64_t;
# 214 "/opt/avr-gcc/avr/include/stdint.h" 3 4
typedef int8_t int_fast8_t;




typedef uint8_t uint_fast8_t;




typedef int16_t int_fast16_t;




typedef uint16_t uint_fast16_t;




typedef int32_t int_fast32_t;




typedef uint32_t uint_fast32_t;







typedef int64_t int_fast64_t;






typedef uint64_t uint_fast64_t;
# 274 "/opt/avr-gcc/avr/include/stdint.h" 3 4
typedef int64_t intmax_t;




typedef uint64_t uintmax_t;
# 10 "/opt/avr-gcc/lib/gcc/avr/4.9.2/include/stdint.h" 2 3 4
# 38 "/opt/avr-gcc/avr/include/inttypes.h" 2 3
# 77 "/opt/avr-gcc/avr/include/inttypes.h" 3
typedef int32_t int_farptr_t;



typedef uint32_t uint_farptr_t;
# 127 "/opt/avr-gcc/avr/include/avr/sfr_defs.h" 2 3
# 100 "/opt/avr-gcc/avr/include/avr/io.h" 2 3
# 390 "/opt/avr-gcc/avr/include/avr/io.h" 3
# 1 "/opt/avr-gcc/avr/include/avr/iotn84.h" 1 3
# 38 "/opt/avr-gcc/avr/include/avr/iotn84.h" 3
# 1 "/opt/avr-gcc/avr/include/avr/iotnx4.h" 1 3
# 39 "/opt/avr-gcc/avr/include/avr/iotn84.h" 2 3
# 391 "/opt/avr-gcc/avr/include/avr/io.h" 2 3
# 534 "/opt/avr-gcc/avr/include/avr/io.h" 3
# 1 "/opt/avr-gcc/avr/include/avr/portpins.h" 1 3
# 535 "/opt/avr-gcc/avr/include/avr/io.h" 2 3

# 1 "/opt/avr-gcc/avr/include/avr/common.h" 1 3
# 537 "/opt/avr-gcc/avr/include/avr/io.h" 2 3

# 1 "/opt/avr-gcc/avr/include/avr/version.h" 1 3
# 539 "/opt/avr-gcc/avr/include/avr/io.h" 2 3






# 1 "/opt/avr-gcc/avr/include/avr/fuse.h" 1 3
# 239 "/opt/avr-gcc/avr/include/avr/fuse.h" 3
typedef struct
{
    unsigned char low;
    unsigned char high;
    unsigned char extended;
} __fuse_t;
# 546 "/opt/avr-gcc/avr/include/avr/io.h" 2 3


# 1 "/opt/avr-gcc/avr/include/avr/lock.h" 1 3
# 549 "/opt/avr-gcc/avr/include/avr/io.h" 2 3
# 39 "/opt/avr-gcc/avr/include/avr/interrupt.h" 2 3
# 2 "main.c" 2

# 1 "/opt/avr-gcc/avr/include/avr/sleep.h" 1 3
# 4 "main.c" 2
# 1 "/opt/avr-gcc/avr/include/avr/power.h" 1 3
# 2636 "/opt/avr-gcc/avr/include/avr/power.h" 3
typedef enum
{
    clock_div_1 = 0,
    clock_div_2 = 1,
    clock_div_4 = 2,
    clock_div_8 = 3,
    clock_div_16 = 4,
    clock_div_32 = 5,
    clock_div_64 = 6,
    clock_div_128 = 7,
    clock_div_256 = 8
} clock_div_t;

static __inline__ void clock_prescale_set(clock_div_t) __attribute__((__always_inline__));

void clock_prescale_set(clock_div_t __x)
{
    uint8_t __tmp = (1 << (7));
    __asm__ __volatile__ (
        "in __tmp_reg__,__SREG__" "\n\t"
        "cli" "\n\t"
        "out %1, %0" "\n\t"
        "out %1, %2" "\n\t"
        "out __SREG__, __tmp_reg__"
        :
        : "d" (__tmp),
          "I" ((((uint16_t) &((*(volatile uint8_t *)((0x26) + 0x20)))) - 0x20)),
          "d" (__x)
        : "r0");
}
# 5 "main.c" 2
# 1 "/opt/avr-gcc/avr/include/avr/wdt.h" 1 3
# 6 "main.c" 2
# 1 "/opt/avr-gcc/avr/include/util/delay.h" 1 3
# 43 "/opt/avr-gcc/avr/include/util/delay.h" 3
# 1 "/opt/avr-gcc/avr/include/util/delay_basic.h" 1 3
# 40 "/opt/avr-gcc/avr/include/util/delay_basic.h" 3
static inline void _delay_loop_1(uint8_t __count) __attribute__((always_inline));
static inline void _delay_loop_2(uint16_t __count) __attribute__((always_inline));
# 80 "/opt/avr-gcc/avr/include/util/delay_basic.h" 3
void
_delay_loop_1(uint8_t __count)
{
 __asm__ volatile (
  "1: dec %0" "\n\t"
  "brne 1b"
  : "=r" (__count)
  : "0" (__count)
 );
}
# 102 "/opt/avr-gcc/avr/include/util/delay_basic.h" 3
void
_delay_loop_2(uint16_t __count)
{
 __asm__ volatile (
  "1: sbiw %0,1" "\n\t"
  "brne 1b"
  : "=w" (__count)
  : "0" (__count)
 );
}
# 44 "/opt/avr-gcc/avr/include/util/delay.h" 2 3
# 1 "/opt/avr-gcc/avr/include/math.h" 1 3
# 127 "/opt/avr-gcc/avr/include/math.h" 3
extern double cos(double __x) __attribute__((__const__));





extern double sin(double __x) __attribute__((__const__));





extern double tan(double __x) __attribute__((__const__));






extern double fabs(double __x) __attribute__((__const__));






extern double fmod(double __x, double __y) __attribute__((__const__));
# 168 "/opt/avr-gcc/avr/include/math.h" 3
extern double modf(double __x, double *__iptr);



extern float modff (float __x, float *__iptr);




extern double sqrt(double __x) __attribute__((__const__));
extern float sqrtf (float) __attribute__((__const__));




extern double cbrt(double __x) __attribute__((__const__));
# 194 "/opt/avr-gcc/avr/include/math.h" 3
extern double hypot (double __x, double __y) __attribute__((__const__));







extern double square(double __x) __attribute__((__const__));






extern double floor(double __x) __attribute__((__const__));






extern double ceil(double __x) __attribute__((__const__));
# 234 "/opt/avr-gcc/avr/include/math.h" 3
extern double frexp(double __x, int *__pexp);







extern double ldexp(double __x, int __exp) __attribute__((__const__));





extern double exp(double __x) __attribute__((__const__));





extern double cosh(double __x) __attribute__((__const__));





extern double sinh(double __x) __attribute__((__const__));





extern double tanh(double __x) __attribute__((__const__));







extern double acos(double __x) __attribute__((__const__));







extern double asin(double __x) __attribute__((__const__));






extern double atan(double __x) __attribute__((__const__));
# 298 "/opt/avr-gcc/avr/include/math.h" 3
extern double atan2(double __y, double __x) __attribute__((__const__));





extern double log(double __x) __attribute__((__const__));





extern double log10(double __x) __attribute__((__const__));





extern double pow(double __x, double __y) __attribute__((__const__));






extern int isnan(double __x) __attribute__((__const__));
# 333 "/opt/avr-gcc/avr/include/math.h" 3
extern int isinf(double __x) __attribute__((__const__));






__attribute__((__const__)) static inline int isfinite (double __x)
{
    unsigned char __exp;
    __asm__ (
 "mov	%0, %C1		\n\t"
 "lsl	%0		\n\t"
 "mov	%0, %D1		\n\t"
 "rol	%0		"
 : "=r" (__exp)
 : "r" (__x) );
    return __exp != 0xff;
}






__attribute__((__const__)) static inline double copysign (double __x, double __y)
{
    __asm__ (
 "bst	%D2, 7	\n\t"
 "bld	%D0, 7	"
 : "=r" (__x)
 : "0" (__x), "r" (__y) );
    return __x;
}
# 376 "/opt/avr-gcc/avr/include/math.h" 3
extern int signbit (double __x) __attribute__((__const__));






extern double fdim (double __x, double __y) __attribute__((__const__));
# 392 "/opt/avr-gcc/avr/include/math.h" 3
extern double fma (double __x, double __y, double __z) __attribute__((__const__));







extern double fmax (double __x, double __y) __attribute__((__const__));







extern double fmin (double __x, double __y) __attribute__((__const__));






extern double trunc (double __x) __attribute__((__const__));
# 426 "/opt/avr-gcc/avr/include/math.h" 3
extern double round (double __x) __attribute__((__const__));
# 439 "/opt/avr-gcc/avr/include/math.h" 3
extern long lround (double __x) __attribute__((__const__));
# 453 "/opt/avr-gcc/avr/include/math.h" 3
extern long lrint (double __x) __attribute__((__const__));
# 45 "/opt/avr-gcc/avr/include/util/delay.h" 2 3
# 84 "/opt/avr-gcc/avr/include/util/delay.h" 3
static inline void _delay_us(double __us) __attribute__((always_inline));
static inline void _delay_ms(double __ms) __attribute__((always_inline));
# 141 "/opt/avr-gcc/avr/include/util/delay.h" 3
void
_delay_ms(double __ms)
{
 double __tmp ;



 uint32_t __ticks_dc;
 extern void __builtin_avr_delay_cycles(unsigned long);
 __tmp = ((1000000) / 1e3) * __ms;
# 160 "/opt/avr-gcc/avr/include/util/delay.h" 3
  __ticks_dc = (uint32_t)(ceil(fabs(__tmp)));


 __builtin_avr_delay_cycles(__ticks_dc);
# 186 "/opt/avr-gcc/avr/include/util/delay.h" 3
}
# 223 "/opt/avr-gcc/avr/include/util/delay.h" 3
void
_delay_us(double __us)
{
 double __tmp ;



 uint32_t __ticks_dc;
 extern void __builtin_avr_delay_cycles(unsigned long);
 __tmp = ((1000000) / 1e6) * __us;
# 242 "/opt/avr-gcc/avr/include/util/delay.h" 3
  __ticks_dc = (uint32_t)(ceil(fabs(__tmp)));


 __builtin_avr_delay_cycles(__ticks_dc);
# 268 "/opt/avr-gcc/avr/include/util/delay.h" 3
}
# 7 "main.c" 2
# 1 "../lib/dbg_uart.h" 1






void dbg_uart_init(void);
# 8 "main.c" 2
# 1 "/opt/avr-gcc/avr/include/stdio.h" 1 3
# 45 "/opt/avr-gcc/avr/include/stdio.h" 3
# 1 "/opt/avr-gcc/lib/gcc/avr/4.9.2/include/stdarg.h" 1 3 4
# 40 "/opt/avr-gcc/lib/gcc/avr/4.9.2/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 98 "/opt/avr-gcc/lib/gcc/avr/4.9.2/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
# 46 "/opt/avr-gcc/avr/include/stdio.h" 2 3



# 1 "/opt/avr-gcc/lib/gcc/avr/4.9.2/include/stddef.h" 1 3 4
# 212 "/opt/avr-gcc/lib/gcc/avr/4.9.2/include/stddef.h" 3 4
typedef unsigned int size_t;
# 50 "/opt/avr-gcc/avr/include/stdio.h" 2 3
# 242 "/opt/avr-gcc/avr/include/stdio.h" 3
struct __file {
 char *buf;
 unsigned char unget;
 uint8_t flags;
# 261 "/opt/avr-gcc/avr/include/stdio.h" 3
 int size;
 int len;
 int (*put)(char, struct __file *);
 int (*get)(struct __file *);
 void *udata;
};
# 275 "/opt/avr-gcc/avr/include/stdio.h" 3
typedef struct __file FILE;
# 405 "/opt/avr-gcc/avr/include/stdio.h" 3
extern struct __file *__iob[];
# 417 "/opt/avr-gcc/avr/include/stdio.h" 3
extern FILE *fdevopen(int (*__put)(char, FILE*), int (*__get)(FILE*));
# 434 "/opt/avr-gcc/avr/include/stdio.h" 3
extern int fclose(FILE *__stream);
# 608 "/opt/avr-gcc/avr/include/stdio.h" 3
extern int vfprintf(FILE *__stream, const char *__fmt, va_list __ap);





extern int vfprintf_P(FILE *__stream, const char *__fmt, va_list __ap);






extern int fputc(int __c, FILE *__stream);




extern int putc(int __c, FILE *__stream);


extern int putchar(int __c);
# 649 "/opt/avr-gcc/avr/include/stdio.h" 3
extern int printf(const char *__fmt, ...);





extern int printf_P(const char *__fmt, ...);







extern int vprintf(const char *__fmt, va_list __ap);





extern int sprintf(char *__s, const char *__fmt, ...);





extern int sprintf_P(char *__s, const char *__fmt, ...);
# 685 "/opt/avr-gcc/avr/include/stdio.h" 3
extern int snprintf(char *__s, size_t __n, const char *__fmt, ...);





extern int snprintf_P(char *__s, size_t __n, const char *__fmt, ...);





extern int vsprintf(char *__s, const char *__fmt, va_list ap);





extern int vsprintf_P(char *__s, const char *__fmt, va_list ap);
# 713 "/opt/avr-gcc/avr/include/stdio.h" 3
extern int vsnprintf(char *__s, size_t __n, const char *__fmt, va_list ap);





extern int vsnprintf_P(char *__s, size_t __n, const char *__fmt, va_list ap);




extern int fprintf(FILE *__stream, const char *__fmt, ...);





extern int fprintf_P(FILE *__stream, const char *__fmt, ...);






extern int fputs(const char *__str, FILE *__stream);




extern int fputs_P(const char *__str, FILE *__stream);





extern int puts(const char *__str);




extern int puts_P(const char *__str);
# 762 "/opt/avr-gcc/avr/include/stdio.h" 3
extern size_t fwrite(const void *__ptr, size_t __size, size_t __nmemb,
         FILE *__stream);







extern int fgetc(FILE *__stream);




extern int getc(FILE *__stream);


extern int getchar(void);
# 810 "/opt/avr-gcc/avr/include/stdio.h" 3
extern int ungetc(int __c, FILE *__stream);
# 822 "/opt/avr-gcc/avr/include/stdio.h" 3
extern char *fgets(char *__str, int __size, FILE *__stream);






extern char *gets(char *__str);
# 840 "/opt/avr-gcc/avr/include/stdio.h" 3
extern size_t fread(void *__ptr, size_t __size, size_t __nmemb,
        FILE *__stream);




extern void clearerr(FILE *__stream);
# 857 "/opt/avr-gcc/avr/include/stdio.h" 3
extern int feof(FILE *__stream);
# 868 "/opt/avr-gcc/avr/include/stdio.h" 3
extern int ferror(FILE *__stream);






extern int vfscanf(FILE *__stream, const char *__fmt, va_list __ap);




extern int vfscanf_P(FILE *__stream, const char *__fmt, va_list __ap);







extern int fscanf(FILE *__stream, const char *__fmt, ...);




extern int fscanf_P(FILE *__stream, const char *__fmt, ...);






extern int scanf(const char *__fmt, ...);




extern int scanf_P(const char *__fmt, ...);







extern int vscanf(const char *__fmt, va_list __ap);







extern int sscanf(const char *__buf, const char *__fmt, ...);




extern int sscanf_P(const char *__buf, const char *__fmt, ...);
# 938 "/opt/avr-gcc/avr/include/stdio.h" 3
static __inline__ int fflush(FILE *stream __attribute__((unused)))
{
 return 0;
}





typedef long long fpos_t;
extern int fgetpos(FILE *stream, fpos_t *pos);
extern FILE *fopen(const char *path, const char *mode);
extern FILE *freopen(const char *path, const char *mode, FILE *stream);
extern FILE *fdopen(int, const char *);
extern int fseek(FILE *stream, long offset, int whence);
extern int fsetpos(FILE *stream, fpos_t *pos);
extern long ftell(FILE *stream);
extern int fileno(FILE *);
extern void perror(const char *s);
extern int remove(const char *pathname);
extern int rename(const char *oldpath, const char *newpath);
extern void rewind(FILE *stream);
extern void setbuf(FILE *stream, char *buf);
extern int setvbuf(FILE *stream, char *buf, int mode, size_t size);
extern FILE *tmpfile(void);
extern char *tmpnam (char *s);
# 9 "main.c" 2

uint16_t sample_vcc(void)
{
 (*(volatile uint8_t *)((0x07) + 0x20)) = (1 << (3)) | (1 << (2));
 (*(volatile uint8_t *)((0x06) + 0x20)) = (1 << (4)) | (1 << (3)) |
  (1 << (1)) | (1 << (0));
 (*(volatile uint8_t *)((0x03) + 0x20)) = 0;

 uint16_t res;

 ((*(volatile uint8_t *)((0x00) + 0x20)) &= (uint8_t)~(1 << 0));
 do { (*(volatile uint8_t *)((0x35) + 0x20)) = (((*(volatile uint8_t *)((0x35) + 0x20)) & ~((1 << (3)) | (1 << (4)))) | ((1 << (3)))); } while(0);
 (*(volatile uint8_t *)((0x06) + 0x20)) |= (1 << (7)) | (1 << (6));
 while((*(volatile uint8_t *)((0x06) + 0x20)) & (1 << (6)))
  do { do { (*(volatile uint8_t *)((0x35) + 0x20)) |= (uint8_t)(1 << (5)); } while(0); do { __asm__ __volatile__ ( "sleep" "\n\t" :: ); } while(0); do { (*(volatile uint8_t *)((0x35) + 0x20)) &= (uint8_t)(~(1 << (5))); } while(0); } while (0);
 res = (*(volatile uint16_t *)((0x04) + 0x20));
 (*(volatile uint8_t *)((0x06) + 0x20)) &= ~((1 << (7)));
 ((*(volatile uint8_t *)((0x00) + 0x20)) |= (uint8_t)(1 << 0));
 return res;
}

uint16_t sample_temperature(void)
{
 (*(volatile uint8_t *)((0x07) + 0x20)) = (1 << (7)) | (1 << (5)) | (1 << (1));
 (*(volatile uint8_t *)((0x06) + 0x20)) = (1 << (4)) | (1 << (3)) |
  (1 << (1)) | (1 << (0));
 (*(volatile uint8_t *)((0x03) + 0x20)) = 0;

 uint16_t res;

 ((*(volatile uint8_t *)((0x00) + 0x20)) &= (uint8_t)~(1 << 0));
 do { (*(volatile uint8_t *)((0x35) + 0x20)) = (((*(volatile uint8_t *)((0x35) + 0x20)) & ~((1 << (3)) | (1 << (4)))) | ((1 << (3)))); } while(0);
 (*(volatile uint8_t *)((0x06) + 0x20)) |= (1 << (7)) | (1 << (6));
 while((*(volatile uint8_t *)((0x06) + 0x20)) & (1 << (6)))
  do { do { (*(volatile uint8_t *)((0x35) + 0x20)) |= (uint8_t)(1 << (5)); } while(0); do { __asm__ __volatile__ ( "sleep" "\n\t" :: ); } while(0); do { (*(volatile uint8_t *)((0x35) + 0x20)) &= (uint8_t)(~(1 << (5))); } while(0); } while (0);
 res = (*(volatile uint16_t *)((0x04) + 0x20));
 (*(volatile uint8_t *)((0x06) + 0x20)) &= ~((1 << (7)));
 ((*(volatile uint8_t *)((0x00) + 0x20)) |= (uint8_t)(1 << 0));
 return res;
}

int main (void)
{
 __asm__ __volatile__ ( "in __tmp_reg__, __SREG__" "\n\t" "cli" "\n\t" "out %0, %1" "\n\t" "out %0, __zero_reg__" "\n\t" "out __SREG__,__tmp_reg__" "\n\t" : : "I" ((((uint16_t) &((*(volatile uint8_t *)((0x21) + 0x20)))) - 0x20)), "r" ((uint8_t)((1 << (4)) | (1 << (3)))) : "r0" );


 dbg_uart_init();
 printf("\r\r\r\r\n\n\n\n\nblink\n");


 (*(volatile uint8_t *)((0x17) + 0x20)) |= (1 << (2));

 uint8_t cnt = 0;
 uint8_t rows = 0;

 while(1)
 {
  (*(volatile uint8_t *)((0x18) + 0x20)) |= (1 << (2));
  _delay_ms(100);

  (*(volatile uint8_t *)((0x18) + 0x20)) &= ~(1 << (2));
  _delay_ms(1000);
  uint16_t v = sample_vcc();
  _delay_ms(1000);
  uint16_t t = sample_temperature();
  printf("v:%u, t:%d\n", v, t+17);
  cnt++;
  if (cnt % 20 == 0)
  {
   printf("=========== %2d\n", ++rows);
   cnt = 0;
  }
 }

 return 0;
}

void __vector_13 (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_13 (void) {
}

void __vector_4 (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_4 (void) {
}
