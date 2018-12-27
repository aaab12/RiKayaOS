#include "umps/libumps.h"
#include "umps/arch.h"
#include "umps/types.h"
#include "stdint.h"

#include "tapeRec.e"
#include "printer.e"

#define MAXSIZE 4096

void main(void)
{
	char buffer[MAXSIZE];
	
	dtpreg_t *tape0_reg = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 0); 
	
	dtpreg_t *print0_reg = (dtpreg_t *) DEV_REG_ADDR(IL_PRINTER, 0);
	
	while (!tape_End(tape0_reg)) {

		tapeReadBlock(buffer, tape0_reg);
		print_puts(buffer, print0_reg);
	
}	
    /* Go to sleep and power off the machine if anything wakes us up */
    WAIT();
    *((volatile unsigned int *) MCTL_POWER) = 0x0FF;
    while (1) ;
}
