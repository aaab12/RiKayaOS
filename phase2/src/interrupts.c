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
	int deviceno_pending = device_number((memaddr*)INTR_CURRENT_BITMAP(INT_DISK));
  dtpreg_t* device_addr = (dtpreg_t*)DEV_ADDRESS(INT_DISK, deviceno_pending);
  device_addr->command = DEV_C_ACK;
  scheduler();
}

void tape_handler(){
	int deviceno_pending = device_number((memaddr*)INTR_CURRENT_BITMAP(INT_TAPE));
  dtpreg_t* device_addr = (dtpreg_t*)DEV_ADDRESS(INT_TAPE, deviceno_pending);
  device_addr->command = DEV_C_ACK;
  scheduler();
}

void network_handler(){
	int deviceno_pending = device_number((memaddr*)INTR_CURRENT_BITMAP(INT_NETWORK));
  dtpreg_t* device_addr = (dtpreg_t*)DEV_ADDRESS(INT_NETWORK, deviceno_pending);
  device_addr->command = DEV_C_ACK;
  scheduler();
}

void printer_handler(){
	int deviceno_pending = device_number((memaddr*)INTR_CURRENT_BITMAP(INT_PRINTER));
  dtpreg_t* device_addr = (dtpreg_t*)DEV_ADDRESS(INT_PRINTER, deviceno_pending);
  device_addr->command = DEV_C_ACK;
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
