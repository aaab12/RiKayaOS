#include "const_rikaya.h"
#include "handler.h"
#include "interrupt.h"
#include <umps/libumps.h>
#include "scheduler.h"
#include "syscall.h"
#include "types_rikaya.h"
#include "utils.h"

extern pcb_t* current_process;

/* SYSCALL/Breakpoint handler */
void sysbk_handler(){
	kernel_mode(current_process); /* Il processo entra in kernel mode */

  /* Processo chiamante */
	state_t* caller_process = (state_t *)SYSBK_OLDAREA;
  /* Il Program Counter del processo chiamante deve essere incrementato di una WORD_SIZE
   * per evitare loop, in quanto le SYSCALL che non comportano la terminazione di un processo
   * fanno tornare il controllo al flusso di esecuzione che ha richiesto la SYSCALL stessa.
   */
	caller_process->pc_epc += WORD_SIZE;

	/* Parametri della SYSCALL */
	U32* arg0 = &(caller_process->reg_a0); /* Numero della SYSCALL */
	U32 arg1 = caller_process->reg_a1;
	U32 arg2 = caller_process->reg_a2;
	U32 arg3 = caller_process->reg_a3;

	U32* status = &(caller_process->status); /* Stato del processo al momento della chiamata (kernel/user mode) */
	int  cause =   (caller_process->cause); /* Causa della SYSCALL (tipo di eccezione sollevato) */

	U32 return_value = 0; /* Valore di ritorno delle syscall non VOID */

	if (CAUSE_EXCCODE_GET(cause) == 8){ /* SYSCALL */
		if ((*status & STATUS_KUp) == 0 ){ /* kernel mode */
			switch (*arg0){
				case GETCPUTIME:
					get_cpu_time((unsigned int *) arg1, (unsigned int *) arg2, (unsigned int *) arg3);
					break;
				case CREATEPROCESS:
					return_value = create_process((state_t *) arg1, (int) arg2, (void **) arg3);
					break;
				case TERMINATEPROCESS:
					return_value = terminate_process((void **) arg1);
					break;
				case VERHOGEN:
					verhogen((int *) arg1);
					break;
				case PASSEREN:
					passeren((int *) arg1);
					break;
				case WAITCLOCK:
					wait_clock();
					break;
				case WAITIO:
					return_value = do_io((unsigned int) arg1, (unsigned int *) arg2, (unsigned int) arg3);
					break;
				case SETTUTOR:
					set_tutor();
					break;
				case SPECPASSUP:
					return_value = spec_passup((int) arg1, (state_t *) arg2, (state_t *) arg3);
					break;
				case GETPID:
					get_pid_ppid((void **) arg1, (void **) arg2);
					break;
				default: /* Delego a handler di livello superiore se specificato */
					if (!current_process->passup[SYSCALL_TYPE])
						terminate_process(0);
					save_state(caller_process, (current_process->passup_oldarea[SYSCALL_TYPE]));
					LDST(current_process->passup_newarea[SYSCALL_TYPE]);
					break;
			}
		} else { /* user mode */
			/* Syscall invocata in user mode, quindi lancio TRAP */
			/* (non entra mai qua, entra direttamente in pgmtrap_handler) */
			pgmtrap_handler();
		}
	} else if (CAUSE_EXCCODE_GET(cause) == 9){ /* BREAKPOINT */
			if (!current_process->passup[SYSCALL_TYPE]) /* Delego a handler di livello superiore se specificato */
				terminate_process(0);
			save_state(caller_process, (current_process->passup_oldarea[SYSCALL_TYPE]));
			LDST(current_process->passup_newarea[SYSCALL_TYPE]);
	}

	caller_process->reg_v0 = return_value; /* Il registro v0 contiene il valore di ritorno */

	if(current_process) /* La PASSEREN aggiorna il tempo passato in Kernel Mode e poi richiama lo scheduler che setta current_process = NULL */
		user_mode(current_process); /* Il processo torna in user mode */

	LDST(caller_process); /* Ritorniamo al flusso di esecuzione */
}

/* Program Traps handler */
void pgmtrap_handler(){
	state_t* caller_process = (state_t *)PGMTRAP_OLDAREA; /* Processo chiamante */
	caller_process->pc_epc += WORD_SIZE;

	if (!current_process->passup[TRAP_TYPE]) /* Delego a handler di livello superiore se specificato */
		terminate_process(0);

	save_state(caller_process, (current_process->passup_oldarea[TRAP_TYPE]));
	LDST(current_process->passup_newarea[TRAP_TYPE]);
}

/* TLB Management handler */
void tlb_handler(){
	state_t* caller_process = (state_t *)TLB_OLDAREA; /* Processo chiamante */
	caller_process->pc_epc += WORD_SIZE;

	if (!current_process->passup[TLB_TYPE]) /* Delego a handler di livello superiore se specificato */
		terminate_process(0);

	save_state(caller_process, (current_process->passup_oldarea[TLB_TYPE]));
	LDST(current_process->passup_newarea[TLB_TYPE]);
}

/* Interrupts handler */
void int_handler(){
	kernel_mode(current_process); /* Il processo entra in kernel mode */

	/* Linea da cui proviene l'interrupt */
	int line = 1;
	while ((line<7) && !(CAUSE_IP_GET(getCAUSE(), line))) {
		line++;
	}
	switch(line){
		case INT_T_SLICE:
			plt_handler(); /* Process Local Timer handler */
			break;
		case INT_TIMER:
			it_handler(); /* Interval Timer handler */
			break;
		case INT_DISK:
			disk_handler();
			break;
		case INT_TAPE:
			tape_handler();
			break;
		case INT_NETWORK:
			network_handler();
			break;
		case INT_PRINTER:
			printer_handler();
			break;
		case INT_TERMINAL:
			terminal_handler();
			break;
		default:
			PANIC(); /* Interrupt non implementato o linea errata */
	}
}
