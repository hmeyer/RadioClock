#include <stdio.h>
extern "C" {

void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

void uart_init(void);
void stream_init(void);

/* http://www.ermicro.com/blog/?p=325 */
}
