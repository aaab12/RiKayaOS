#include "interrupt.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;
extern int clock_semaphore;
extern int clock_semaphore_counter;
extern int terminal_semaphore[DEV_PER_INT][2];

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
	int deviceno_pending = device_number((memaddr*)INTR_CURRENT_BITMAP(INT_TERMINAL));
	termreg_t* device_addr = (termreg_t*)DEV_REG_ADDR(INT_TERMINAL, deviceno_pending);
	if((device_addr->recv_status != TERM_ST_BUSY) && (device_addr->recv_status !=  DEV_S_READY)){
		pcb_t* unblocked = V_pcb(&terminal_semaphore[deviceno_pending][0]);
		if(unblocked){
			unblocked->p_s.reg_v0 = device_addr->recv_status;
			insertProcQ(&ready_queue, unblocked);
		}
		device_addr->recv_command = DEV_C_ACK;
		while((device_addr->recv_status & TERM_STATUS_MASK) != DEV_S_READY);
	}

	if((device_addr->transm_status != TERM_ST_BUSY) && (device_addr->transm_status != DEV_S_READY)){
		pcb_t* unblocked = V_pcb(&terminal_semaphore[deviceno_pending][1]);
		if(unblocked){
			unblocked->p_s.reg_v0 = device_addr->transm_status;
			insertProcQ(&ready_queue, unblocked);
		}
		device_addr->transm_command = DEV_C_ACK;
		while((device_addr->transm_status & TERM_STATUS_MASK) != DEV_S_READY);
	}

	if(current_process){
		//    user_mode(current_process); /* Il processo torna in user mode */
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		insertProcQ(&ready_queue, current_process);
	}

	scheduler();
}
