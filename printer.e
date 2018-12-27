#ifndef PRINTER_E
#define PRINTER_E

extern void print_puts(const char *str, dtpreg_t *dtp);

extern int print_putchar(char c, dtpreg_t *dtp);

extern uint32_t printer_status(dtpreg_t *dtp);

#endif
