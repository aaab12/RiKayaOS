#include "umps/arch.h"
#include "umps/types.h"
#include "stdint.h"
#include "tape.h"
#include "tape.e"

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

/* Skip a single block on the tape */
extern void tape_skip(dtpreg_t *dtp)
{
				dtp->command = CMD_SKIPBLK;
}

/* Boolean to check if the tape is endend */
extern uint8_t tape_end(dtpreg_t *dtp)
{
				return(dtp->data1 == MK_EOT);
}
