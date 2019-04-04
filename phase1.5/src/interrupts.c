#include <umps/libumps.h>
#include "const.h"
#include "listx.h"
#include "pcb.h"
#include "interrupt.h"
#include "main.h"
#include "scheduler.h"
#include "utils.h"

void plt_handler() {
	//termprint("\n==> nel plt handler\n", 1);
	state_t* caller_process = (state_t *)INT_OLDAREA;
	save_state(caller_process, &(current_process->p_s));
	insertProcQ(&ready_queue, current_process);
	//termprint("==> passo controllo a scheduler\n", 1);
	scheduler();
	// if(current_process != NULL && current_process->cpu_slice >=SCHED_TIME_SLICE) {
	// 	insertProcQ(&ready_queue, current_process);
	// 	current_process = NULL;
	// 	setTIMER(SCHED_TIME_SLICE);
	// }
}
