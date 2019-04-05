#include <umps/libumps.h>
#include "const.h"
#include "listx.h"
#include "pcb.h"
#include "interrupt.h"
#include "scheduler.h"
#include "utils.h"
#include "main.h"

void plt_handler() {
	state_t* caller_process = (state_t *)INT_OLDAREA;
	save_state(caller_process, &(current_process->p_s));
	insertProcQ(&ready_queue, current_process);
	setTIMER(TIME_SLICE); /* ACK */
	scheduler();
}
