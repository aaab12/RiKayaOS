#include "interrupt.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;

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

void disk_handler(){

}

void tape_handler(){

}

void network_handler(){

}

void printer_handler(){

}

void terminal_handler(){

}
