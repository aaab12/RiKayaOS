#include "tapeRec.h"
#include "tapeRec.e"
#include "umps/arch.h"
#include "stdint.h"

extern uint32_t tapeReadBlock(char *buffer, dtpreg_t *dtp)
{
	uint32_t stat;
	
	if (dtp->status != ST_READY)
		return dtp->status;

	dtp->data0 = (uint32_t) buffer;
	dtp->command = CMD_READBLK;

	/* Wait for execution */
	while (dtp->status == ST_BUSY);
	
	stat = dtp->status;
	dtp->command = CMD_ACK;
		
	return stat;

}

extern void tapeSkipBlock(dtpreg_t *dtp) {
	dtp->command = CMD_SKIPBLK;
}

extern uint8_t tape_End(dtpreg_t *dtp) {
	return(dtp->data1 == MK_EOT);
}


