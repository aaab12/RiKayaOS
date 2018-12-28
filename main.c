#include "umps/libumps.h"
#include "umps/arch.h"
#include "umps/types.h"
#include "stdint.h"
#include "tape.e"
#include "printer.e"

#define MAXSIZE 4096

static void halt(void);
void tirulero_maker_1(dtpreg_t *names, dtpreg_t *refrain, dtpreg_t *words, dtpreg_t *printer);
void tirulero_maker_2(dtpreg_t *song, dtpreg_t *printer);
void AllTogether(char *buffer, dtpreg_t *nameOfTape, dtpreg_t *nameOfPrinter, int nRewind, int nSkip, int nBack);

void main(void)
{
				/* Declaration of the tapes */
				dtpreg_t *tape0_reg = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 0);
				dtpreg_t *tape1_reg = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 1);
				dtpreg_t *tape2_reg = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 2);
				dtpreg_t *tape3_reg = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 3);

				/* Declaration of the printers */
				dtpreg_t *print0_reg = (dtpreg_t *) DEV_REG_ADDR(IL_PRINTER, 0);
				dtpreg_t *print1_reg = (dtpreg_t *) DEV_REG_ADDR(IL_PRINTER, 1);

				tirulero_maker_1(tape0_reg, tape1_reg, tape2_reg, print0_reg);
				tirulero_maker_2(tape3_reg, print1_reg);

				/* Go to sleep and power off the machine if anything wakes us up */
				halt();
}



static void halt(void)
{
				WAIT();
				*((volatile uint32_t *) MCTL_POWER) = 0x0FF;
				while (1);
}

void tirulero_maker_1(dtpreg_t *names, dtpreg_t *refrain, dtpreg_t *words, dtpreg_t *printer)
{
				char buffer[MAXSIZE];

				/* First occurrence of the names and rewind */
				AllTogether(buffer, names, printer, 1, 0, 0);

				/* First stanza of the song */
				AllTogether(buffer, words, printer, 0, 0, 0);

				/* First occurrence of the refrain and rewind */
				AllTogether(buffer, refrain, printer, 1, 0, 0);

				/* Second stanza of the song */
				AllTogether(buffer, words, printer, 0, 0, 0);

				/* Second occurrence of the names */
				AllTogether(buffer, names, printer, 0, 0, 0);

				/* Third stanza of the song */
				AllTogether(buffer, words, printer, 0, 0, 0);

				/* Second occurrence of the refrain and rewind */
				AllTogether(buffer, refrain, printer, 1, 0, 0);
				
				/* Third occurrence of the refrain */
				AllTogether(buffer, refrain, printer, 0, 0, 0);
}

void tirulero_maker_2(dtpreg_t *song, dtpreg_t *printer)
{
				char buffer[MAXSIZE];

				/* First block with the names and skip the block with the refrain */
				AllTogether(buffer, song, printer, 0, 1, 0);

				/* Third block with the first stanza and back two blocks at the beginning of the block with the refrain */
				AllTogether(buffer, song, printer, 0, 0, 2);

				/* Second block with the refrain and skip the third block */
				AllTogether(buffer, song, printer, 0, 1, 0);

				/* The fourth block with the second stanza and rewind the entire tape */
				AllTogether(buffer, song, printer, 1, 0, 0);

				/* First block with the names and skip the block with the refrain and the other two blocks
					 with the first and the second stanza */
				AllTogether(buffer, song, printer, 0, 3, 0); 

				/* The fifth block with the third stanza, rewind the entire tape and skip the block with the names */
				AllTogether(buffer, song, printer, 1, 1, 0);

				/* Second block with the refrain and back one block at the beginning of the block with the refrain*/
				AllTogether(buffer, song, printer, 0, 0, 1);

				/* Second block with the refrain */
				AllTogether(buffer, song, printer, 0, 0, 0);
}

void AllTogether(char *buffer, dtpreg_t *nameOfTape, dtpreg_t *nameOfPrinter, int nRewind, int nSkip, int nBack)
{
				tape_read(buffer, nameOfTape);
				print_puts(buffer, nameOfPrinter);

				/* Rewind the entire tape */
				while(nRewind) 
				{
					tape_rewind(nameOfTape);
					nRewind--;
				}
				/* Skip the block with the names */
				while(nSkip)
				{
					tape_skip(nameOfTape);
					nSkip--;
				}			
				/* Back one block at the beginning of the block with the tape */		
				while(nBack) 
				{
					tape_back(nameOfTape);
					nBack--;
				}
}
	
