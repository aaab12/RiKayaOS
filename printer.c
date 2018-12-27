#include "printer.h"
#include "printer.e"
#include "umps/arch.h"
#include "stdint.h"

extern void print_puts(const char *str, dtpreg_t *dtp)
{
    while (*str)
        if (print_putchar(*str++, dtp))
            return;
}

extern int print_putchar(char c, dtpreg_t *dtp)
{
    uint32_t stat;

    stat = printer_status(dtp);
    if (stat != ST_READY)
        return -1;

    dtp->data0 = c ;
    dtp->command = CMD_PRINTCHR; 

    while ((stat = printer_status(dtp)) == ST_BUSY)
        ;

    dtp->command = CMD_ACK;

    return 0;
}

extern uint32_t printer_status(dtpreg_t *dtp)
{
    return ((dtp->status) & PRINTER_STATUS_MASK);
}
