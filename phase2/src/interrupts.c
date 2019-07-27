#include "const.h"
#include "const_rikaya.h"
#include "interrupt.h"
#include <umps/libumps.h>
#include "main.h"
#include "scheduler.h"
#include "types_rikaya.h"
#include "utils.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;
extern int clock_semaphore;
extern int clock_semaphore_counter;

/* Process Local Timer handler */
void plt_handler() {
	if(current_process){
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}
	setTIMER(TIME_SLICE); /* ACK */
	scheduler(); /* Il controllo va allo scheduler */
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
	scheduler(); /* Il controllo va allo scheduler */
}

/* Disk handler */
void disk_handler(){
  for (int device = 0; device < 8; device++){ /* Cerco i device disk che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_DISK - 3)); /* ...mi interessano quelli con interrupt pending */
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_DISK, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
		}
	}

  if(current_process){ /* Se c'era un processo in stato running ne salvo lo stato e lo rimetto nella coda dei processi ready */
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}
	scheduler(); /* Il controllo va allo scheduler */
}

/*Tape handler */
void tape_handler(){
	for (int device = 0; device < 8; device++){ /* Cerco i device tape che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_TAPE - 3)); /* ...mi interessano quelli con interrupt pending */
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_TAPE, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
		}
	}

	if(current_process){ /* Se c'era un processo in stato running ne salvo lo stato e lo rimetto nella coda dei processi ready */
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}
	scheduler(); /* Il controllo va allo scheduler */
}

/* Network handler */
void network_handler(){
	for (int device = 0; device < 8; device++){ /* Cerco i device network che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_NETWORK - 3)); /* ...mi interessano quelli con interrupt pending */
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_NETWORK, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
		}
	}

	if(current_process){ /* Se c'era un processo in stato running ne salvo lo stato e lo rimetto nella coda dei processi ready */
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}
	scheduler(); /* Il controllo va allo scheduler */
}

/* Printer handler */
void printer_handler(){
	for (int device = 0; device < 8; device++){ /* Cerco i device printer che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_PRINTER - 3)); /* ...mi interessano quelli con interrupt pending */
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_PRINTER, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
		}
	}

	if(current_process){ /* Se c'era un processo in stato running ne salvo lo stato e lo rimetto nella coda dei processi ready */
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}
	scheduler(); /* Il controllo va allo scheduler */
}

/* Terminal handler */
void terminal_handler(){
	for (int device = 0; device < 8; device++){ /* Cerco i device terminal che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_TERMINAL - 3)); /* ...mi interessano quelli con interrupt pending */
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_TERMINAL, device);

			/* Caso recive */
			if((((termreg_t *)device_addr)->recv_status != TERM_ST_BUSY) && (((termreg_t *)device_addr)->recv_status !=  DEV_S_READY)){
			  ((termreg_t *)device_addr)->recv_command = DEV_C_ACK; /* Faccio l'ack */
			  while((((termreg_t *)device_addr)->recv_status & TERM_STATUS_MASK) == TERM_ST_BUSY); /* Attendo che lo status torni ready */
			}

			/* Caso transmitted */
			if((((termreg_t *)device_addr)->transm_status != TERM_ST_BUSY) && (((termreg_t *)device_addr)->transm_status != DEV_S_READY)){
				((termreg_t *)device_addr)->transm_command = DEV_C_ACK; /* Faccio l'ack */
				while((((termreg_t *)device_addr)->transm_status & TERM_STATUS_MASK) == TERM_ST_BUSY); /* Attendo che lo status torni ready */
			}
		}
	}

	if(current_process){ /* Se c'era un processo in stato running ne salvo lo stato e lo rimetto nella coda dei processi ready */
		state_t* caller_process = (state_t *)INT_OLDAREA;
		save_state(caller_process, &(current_process->p_s));
		user_mode(current_process); /* Il processo torna in user mode */
		insertProcQ(&ready_queue, current_process); /* Reinserimento del processo interrotto in stato ready */
	}
	scheduler(); /* Il controllo va allo scheduler */
}
