#include "umps/arch.h"
#include "umps/types.h"
#include "stdint.h"
#include "tape.h"
#include "tape.e"

extern int tape_cmd(dtpreg_t *dtp, int cmd)
{
				/* If the status of the tape is not "READY" return */
				if (dtp->status != ST_READY)
								return -1;

				/* Start the skipping operation */
				dtp->command = cmd;

				/* Wait until the status of the tape is not "BUSY" */
				while (dtp->status == ST_BUSY);

				/* Acknowledge the pending interrupt */
				dtp->command = CMD_ACK;

				return 0;
}

/* Rewind the entire tape to the starting point */
extern int tape_rewind(dtpreg_t *dtp)
{
				return tape_cmd(dtp, CMD_RESET);
}

/* Skip a single block on the tape */
extern int tape_skip(dtpreg_t *dtp)
{
				return tape_cmd(dtp, CMD_SKIPBLK);
}

/* Rewind the tape to the previous block */
extern int tape_back(dtpreg_t *dtp)
{
				return tape_cmd(dtp, CMD_BACKBLK);
}

/* Read a single block from the tape */
extern int tape_read(char *buffer, dtpreg_t *dtp)
{
				/* If the status of the tape is not "READY" return */
				if (dtp->status != ST_READY)
			  				return -1;

				/* Set the physical address where to write the block */
				dtp->data0 = (uint32_t) buffer;
				/* Start the reading operation */
				dtp->command = CMD_READBLK;

				/* Wait until the status of the tape is not "BUSY" */
				while (dtp->status == ST_BUSY);

				/* Acknowledge the pending interrupt */
				dtp->command = CMD_ACK;

				return 0;
}

/* Boolean to check if the tape is endend */
extern uint8_t tape_eot(dtpreg_t *dtp)
{
				return(dtp->data1 == MK_EOT);
}

/* Boolean to check if the file is endend */
extern uint8_t tape_eof(dtpreg_t *dtp)
{
				return(dtp->data1 == MK_EOF);
}

/* Boolean to check if the block is endend */
extern uint8_t tape_eob(dtpreg_t *dtp)
{
				return(dtp->data1 == MK_EOB);
}
