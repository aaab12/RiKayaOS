#include <umps/arch.h>
#include "const.h"
#include "const_rikaya.h"
#include "interrupt.h"
#include <umps/libumps.h>
#include "main.h"
#include "pcb.h"
#include "scheduler.h"
#include "syscall.h"
#include "types_rikaya.h"
#include "utils.h"

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
	pcb_t* pcb;
  for (int device = 0; device < 8; device++){ /* Cerco i device disk che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_DISK - 3)); /* ...mi interessano quelli con interrupt pending */
		pending = *((uint32_t *)pending) << (1 << device);
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_DISK, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
			pcb = verhogen_2(&device_semaphore[device]); /* sblocco eventuali processi bloccati sul semaforo del device e aggiorno il semaforo */
			if (pcb) pcb->p_s.reg_v0 = ((dtpreg_t *)device_addr)->status; /* aggiorno il registro v0 del processo sbloccato */
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
	pcb_t* pcb;
	for (int device = 0; device < 8; device++){ /* Cerco i device tape che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_TAPE - 3)); /* ...mi interessano quelli con interrupt pending */
		pending = *((uint32_t *)pending) << (1 << device);
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_TAPE, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK;
			pcb = verhogen_2(&device_semaphore[DEV_PER_INT + device]); /* sblocco eventuali processi bloccati sul semaforo del device e aggiorno il semaforo */
			if (pcb) pcb->p_s.reg_v0 = ((dtpreg_t *)device_addr)->status; /* aggiorno il registro v0 del processo sbloccato */
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
	pcb_t* pcb;
	for (int device = 0; device < 8; device++){ /* Cerco i device network che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_NETWORK - 3)); /* ...mi interessano quelli con interrupt pending */
		pending = *((uint32_t *)pending) << (1 << device);
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_NETWORK, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK; 
			pcb = verhogen_2(&device_semaphore[2 * DEV_PER_INT + device]); /* sblocco eventuali processi bloccati sul semaforo del device e aggiorno il semaforo */
			if (pcb) pcb->p_s.reg_v0 = ((dtpreg_t *)device_addr)->status; /* aggiorno il registro v0 del processo sbloccato */
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
	pcb_t* pcb;
	for (int device = 0; device < 8; device++){ /* Cerco i device printer che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_PRINTER - 3)); /* ...mi interessano quelli con interrupt pending */
		pending = *((unsigned int *)pending) << (1 << device);
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_PRINTER, device);
			((dtpreg_t *)device_addr)->command = (uint8_t) DEV_C_ACK; 
			pcb = verhogen_2(&device_semaphore[3 * DEV_PER_INT + device]); /* sblocco eventuali processi bloccati sul semaforo del device e aggiorno il semaforo */
			if (pcb) pcb->p_s.reg_v0 = ((dtpreg_t *)device_addr)->status; /* aggiorno il registro v0 del processo sbloccato */
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
	pcb_t* pcb;
	for (int device = 0; device < 8; device++){ /* Cerco i device terminal che hanno interrupt pending: per ogni sotto-device da 0 a 7... */
		int pending = PENDING_BITMAP_START + (WORD_SIZE * (INT_TERMINAL - 3)); /* ...mi interessano quelli con interrupt pending */
		pending = *((uint32_t *)pending) << (1 << device);
		if (pending){ /* ...per ognuno di questi faccio l'ack dopo averne trovato l'indirizzo */
			unsigned int device_addr = DEV_REG_ADDR(INT_TERMINAL, device);

			/* Caso receive */
			if((((termreg_t *)device_addr)->recv_status != TERM_ST_BUSY) && (((termreg_t *)device_addr)->recv_status !=  DEV_S_READY)){
			  pcb = verhogen_2(&terminal_semaphore[device][1]); /* sblocco eventuali processi bloccati sul semaforo del device e aggiorno il semaforo */
			  if (pcb) pcb->p_s.reg_v0 = ((termreg_t *)device_addr)->recv_status; /* aggiorno il registro v0 del processo sbloccato (prima dell'ack) */
			  ((termreg_t *)device_addr)->recv_command = DEV_C_ACK; /* Faccio l'ack */
			  while((((termreg_t *)device_addr)->recv_status & TERM_STATUS_MASK) != DEV_S_READY) /* Aspetta finchè lo stato del terminale non è "READY" */
					;
			}

			/* Caso transmit */
			if((((termreg_t *)device_addr)->transm_status != TERM_ST_BUSY) && (((termreg_t *)device_addr)->transm_status != DEV_S_READY)){
			  pcb = verhogen_2(&terminal_semaphore[device][0]); /* sblocco eventuali processi bloccati sul semaforo del device e aggiorno il semaforo */
			  if (pcb) pcb->p_s.reg_v0 = ((termreg_t *)device_addr)->transm_status; /* aggiorno il registro v0 del processo sbloccato (prima dell'ack)*/
				((termreg_t *)device_addr)->transm_command = DEV_C_ACK; /* Faccio l'ack */
				while((((termreg_t *)device_addr)->transm_status & TERM_STATUS_MASK) != DEV_S_READY) /* Aspetta finchè lo stato del terminale non è "READY" */
					;
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
