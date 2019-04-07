#include "interrupt.h"

/* Process Local Timer handler */
void plt_handler() {
	state_t* caller_process = (state_t *)INT_OLDAREA;
	save_state(caller_process, &(current_process->p_s));
	insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	setTIMER(TIME_SLICE); /* ACK */
	scheduler();
}

/* Interval Timer handler */
void it_handler(){
	state_t* caller_process = (state_t *)INT_OLDAREA;
	save_state(caller_process, &(current_process->p_s));
	insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	setIT(TIME_SLICE); /* ACK */
	scheduler();
}
