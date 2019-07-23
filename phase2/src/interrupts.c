#include "interrupt.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;
extern int clock_semaphore;
extern int clock_semaphore_counter;

/* Process Local Timer handler */
void plt_handler() {
	state_t* caller_process = (state_t *)INT_OLDAREA;
	save_state(caller_process, &(current_process->p_s));
	user_mode(current_process); /* Il processo torna in user mode */
	while(clock_semaphore_counter){ /* Sblocca tutti i processi bloccati sul semaforo del clock */
		verhogen(&clock_semaphore); /* V sul semaforo del clock */
		clock_semaphore_counter--;
	}
	insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	setTIMER(TIME_SLICE); /* ACK */
	scheduler();
}

/* Interval Timer handler */
void it_handler(){
	state_t* caller_process = (state_t *)INT_OLDAREA;
	save_state(caller_process, &(current_process->p_s));
	user_mode(current_process); /* Il processo torna in user mode */
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
