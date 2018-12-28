#include "umps/arch.h"
#include "stdint.h"
#include "printer.h"
#include "printer.e"

/* Iterate through the string */
extern void print_puts(const char *str, dtpreg_t *dtp)
{
        while (*str)
                if (print_putchar(*str++, dtp))
                        return;
}

/* Transmit a single char to the printer */
extern int print_putchar(char c, dtpreg_t *dtp)
{
        /* If the status of the printer is not "READY" return */
        if (dtp->status != ST_READY)
                return -1;

        /* Set the character to be trasmitted */
        dtp->data0 = c;
        /* Start the printing operation */
        dtp->command = CMD_PRINTCHR;

        /* Wait until the status of the printer is not "BUSY" */
        while (dtp->status == ST_BUSY);

        /* Acknowledge the pending interrupt */
        dtp->command = CMD_ACK;

        return 0;
}
