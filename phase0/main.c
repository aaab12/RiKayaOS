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
void sing_all_together(char *buffer, dtpreg_t *tape, dtpreg_t *printer, int n_rewind, int n_skip, int n_back);

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
				sing_all_together(buffer, names, printer, 1, 0, 0);

				/* First stanza of the song */
				sing_all_together(buffer, words, printer, 0, 0, 0);

				/* First occurrence of the refrain and rewind */
				sing_all_together(buffer, refrain, printer, 1, 0, 0);

				/* Second stanza of the song */
				sing_all_together(buffer, words, printer, 0, 0, 0);

				/* Second occurrence of the names */
				sing_all_together(buffer, names, printer, 0, 0, 0);

				/* Third stanza of the song */
				sing_all_together(buffer, words, printer, 0, 0, 0);

				/* Second occurrence of the refrain and rewind */
				sing_all_together(buffer, refrain, printer, 1, 0, 0);

				/* Third occurrence of the refrain */
				sing_all_together(buffer, refrain, printer, 0, 0, 0);
}

void tirulero_maker_2(dtpreg_t *song, dtpreg_t *printer)
{
				char buffer[MAXSIZE];

				/* First block with the names and skip the block with the refrain */
				sing_all_together(buffer, song, printer, 0, 1, 0);

				/* Third block with the first stanza and back two blocks at the beginning of the block with the refrain */
				sing_all_together(buffer, song, printer, 0, 0, 2);

				/* Second block with the refrain and skip the third block */
				sing_all_together(buffer, song, printer, 0, 1, 0);

				/* The fourth block with the second stanza and rewind the entire tape */
				sing_all_together(buffer, song, printer, 1, 0, 0);

				/* First block with the names and skip the block with the refrain and the other two blocks
					 with the first and the second stanza */
				sing_all_together(buffer, song, printer, 0, 3, 0);

				/* The fifth block with the third stanza, rewind the entire tape and skip the block with the names */
				sing_all_together(buffer, song, printer, 1, 1, 0);

				/* Second block with the refrain and back one block at the beginning of the block with the refrain*/
				sing_all_together(buffer, song, printer, 0, 0, 1);

				/* Second block with the refrain */
				sing_all_together(buffer, song, printer, 0, 0, 0);
}

void sing_all_together(char *buffer, dtpreg_t *tape, dtpreg_t *printer, int n_rewind, int n_skip, int n_back)
{
				tape_read(buffer, tape);
				print_puts(buffer, printer);

				/* Rewind the entire tape */
				while(n_rewind)
				{
					tape_rewind(tape);
					n_rewind--;
				}

				/* Skip a block */
				while(n_skip)
				{
					tape_skip(tape);
					n_skip--;
				}

				/* Back one block */
				while(n_back)
				{
					tape_back(tape);
					n_back--;
				}
}
