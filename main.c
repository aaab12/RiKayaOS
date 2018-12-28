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

void tirulero_maker_1(dtpreg_t *names, dtpreg_t *refrain, dtpreg_t *words, dtpreg_t *printer)
{
				char buffer[MAXSIZE];

				/* First occurrence of the names */
				tape_read(buffer, names);
				print_puts(buffer, printer);
				/* Rewind the tape with the names */
				tape_rewind(names);

				/* First stanza of the song */
				tape_read(buffer, words);
				print_puts(buffer, printer);

				/* First occurrence of the refrain */
				tape_read(buffer, refrain);
				print_puts(buffer, printer);
				/* Rewind the tape with the refrain */
				tape_rewind(refrain);

				/* Second stanza of the song */
				tape_read(buffer, words);
				print_puts(buffer, printer);

				/* Second occurrence of the names */
				tape_read(buffer, names);
				print_puts(buffer, printer);

				/* Third stanza of the song */
				tape_read(buffer, words);
				print_puts(buffer, printer);

				/* Second occurrence of the refrain */
				tape_read(buffer, refrain);
				print_puts(buffer, printer);
				/* Rewind the tape with the refrain */
				tape_rewind(refrain);

				/* Third occurrence of the refrain */
				tape_read(buffer, refrain);
				print_puts(buffer, printer);
}

void tirulero_maker_2(dtpreg_t *song, dtpreg_t *printer)
{
				char buffer[MAXSIZE];

				/* First block with the names */
				tape_read(buffer, song);
				print_puts(buffer, printer);

				/* Skip the block with the refrain */
				tape_skip(song);

				/* Third block with the first stanza */
				tape_read(buffer, song);
				print_puts(buffer, printer);

				/* Back two blocks at the beginning of the block with the refrain */
				tape_back(song);
				tape_back(song);

				/* Second block with the refrain */
				tape_read(buffer, song);
				print_puts(buffer, printer);

				/* Skip the third block */
				tape_skip(song);

				/* The fourth block with the second stanza */
				tape_read(buffer, song);
				print_puts(buffer, printer);

				/* Rewind the entire tape */
				tape_rewind(song);

				/* First block with the names */
				tape_read(buffer, song);
				print_puts(buffer, printer);

				/* Skip the block with the refrain and the other two blocks
					 with the first and the second stanza */
				tape_skip(song);
				tape_skip(song);
				tape_skip(song);

				/* The fifth block with the third stanza */
				tape_read(buffer, song);
				print_puts(buffer, printer);

				/* Rewind the entire tape */
				tape_rewind(song);
				/* Skip the block with the names */
				tape_skip(song);

				/* Second block with the refrain */
				tape_read(buffer, song);
				print_puts(buffer, printer);

				/* Back one block at the beginning of the block with the refrain */
				tape_back(song);

				/* Second block with the refrain */
				tape_read(buffer, song);
				print_puts(buffer, printer);
}

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
