#include "interrupt.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;
extern int clock_semaphore;
extern int clock_semaphore_counter;
extern int device_semaphore[DEV_PER_INT*(DEV_USED_INTS-1)];
extern int terminal_semaphore[DEV_PER_INT][2];

/* Process Local Timer handler */
void plt_handler() {
	if(current_process){
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}
	setTIMER(TIME_SLICE); /* ACK */
	scheduler();
}

/* Interval Timer handler */
void it_handler(){
	if(current_process){
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}
	while(clock_semaphore_counter){ /* Sblocca tutti i processi bloccati sul semaforo del clock */
		clock_semaphore_counter--;
		verhogen(&clock_semaphore); /* V sul semaforo del clock */
	}
	setIT(SYSTEM_CLOCK); /* ACK */
	scheduler();
}

void disk_handler(){
	for (int device = 0; device < 8; device++){
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_DISK - 3));
		if (pending){
			unsigned int device_addr = DEV_REG_ADDR(INT_DISK, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
		}
	}

	if(current_process){
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}

  scheduler();
}

void tape_handler(){
	for (int device = 0; device < 8; device++){
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_TAPE - 3));
		if (pending){
			unsigned int device_addr = DEV_REG_ADDR(INT_TAPE, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
		}
	}

	if(current_process){
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}

  scheduler();
}

void network_handler(){
	for (int device = 0; device < 8; device++){
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_NETWORK - 3));
		if (pending){
			unsigned int device_addr = DEV_REG_ADDR(INT_NETWORK, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
		}
	}

	if(current_process){
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}

  scheduler();
}

void printer_handler(){
	for (int device = 0; device < 8; device++){
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_PRINTER - 3));
		if (pending){
			unsigned int device_addr = DEV_REG_ADDR(INT_PRINTER, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
		}
	}

	if(current_process){
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}

  scheduler();
}

void terminal_handler(){
	int deviceno_pending = device_number((memaddr*)INTR_CURRENT_BITMAP(INT_TERMINAL));
	termreg_t* device_addr = (termreg_t*)DEV_REG_ADDR(INT_TERMINAL, deviceno_pending);
	if((device_addr->recv_status != TERM_ST_BUSY) && (device_addr->recv_status !=  DEV_S_READY)){
		device_addr->recv_command = DEV_C_ACK;
		while((device_addr->recv_status & TERM_STATUS_MASK) != DEV_S_READY);
	}

	if((device_addr->transm_status != TERM_ST_BUSY) && (device_addr->transm_status != DEV_S_READY)){
		device_addr->transm_command = DEV_C_ACK;
		while((device_addr->transm_status & TERM_STATUS_MASK) != DEV_S_READY);
	}

	if(current_process){
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}

	scheduler();
}
