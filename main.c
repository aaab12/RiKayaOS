#include "umps/libumps.h"
#include "umps/arch.h"
#include "umps/types.h"
#include "stdint.h"
#include "tape.e"
#include "printer.e"

#define MAXSIZE 4096

static void halt(void)
{
				WAIT();
				*((volatile uint32_t *) MCTL_POWER) = 0x0FF;
				while (1);
}

void main(void)
{
				char buffer[MAXSIZE];

				/* Declaration of the tape */
				dtpreg_t *tape0_reg = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 0);

				/* Declaration of the printer */
				dtpreg_t *print0_reg = (dtpreg_t *) DEV_REG_ADDR(IL_PRINTER, 0);

				/* While the tape is not ended */
				while (!tape_end(tape0_reg))
				{
								/* Read a single block from the tape */
								tape_read(buffer, tape0_reg);
								/* Write the block into the printer */
								print_puts(buffer, print0_reg);
				}

				/* Go to sleep and power off the machine if anything wakes us up */
				halt();
}
