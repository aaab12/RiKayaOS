#ifndef PRINTER_E
#define PRINTER_E

#include "umps/types.h"

extern void print_puts(const char *str, dtpreg_t *dtp);

extern int print_putchar(char c, dtpreg_t *dtp);

#endif
