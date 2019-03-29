#include <umps/libumps.h>
#include "const.h"
#include "listx.h"
#include "pcb.h"
#include "interrupt.h"

void int_timer() {
		if(current_process != NULL && current_process->cpu_slice >=SCHED_TIME_SLICE) {
			insertProcQ(&ready_queue, current_process);
			current_process = NULL;
			setTIMER(SCHED_TIME_SLICE);
		}
}
