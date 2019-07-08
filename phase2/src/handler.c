#include "handler.h"

/* SYSCALL/Breakpoint handler */
void sysbk_handler(){
  /* Processo chiamante */
	state_t* caller_process = (state_t *)SYSBK_OLDAREA;
  /* Il Program Counter del processo chiamante deve essere incrementato di una WORD_SIZE
   * per evitare loop, in quanto le SYSCALL che non comportano la terminazione di un processo
   * fanno tornare il controllo al flusso di esecuzione che ha richiesto la SYSCALL stessa.
   */
	caller_process->pc_epc += WORD_SIZE;

	/* Parametri della SYSCALL */
	U32* arg0 = &(caller_process->reg_a0); /* Numero della SYSCALL */
	U32* arg1 = &(caller_process->reg_a1);
	U32* arg2 = &(caller_process->reg_a2);
	U32* arg3	= &(caller_process->reg_a3);

	U32* status = &(caller_process->status); /* Stato del processo al momento della chiamata (kernel/user mode) */
	int  cause =   (caller_process->cause); /* Causa della SYSCALL (tipo di eccezione sollevato) */

	if (CAUSE_EXCCODE_GET(cause) == 8){ /* SYSCALL */
		if ((*status & STATUS_KUp) == 0 ){ /* kernel mode */
			switch (*arg0){
				case GETCPUTIME:
					//get_cpu_time(arg1, arg2, arg3);
					break;
				case CREATEPROCESS:
					//create_process((state_t) *arg1, (int) arg2, (void **) arg3);
					break;
				case TERMINATEPROCESS:
					//terminate_process();
					break;
				case VERHOGEN:
					//verhogen((int) arg1);
					break;
				case PASSEREN:
					//passeren((int) arg1);
					break;
				case WAITCLOCK:
					//wait_clock();
					break;
				case WAITIO:
					//do_io(*arg1, arg2);
					break;
				case SETTUTOR:
					//set_tutor();
					break;
				case SPECPASSUP:
					//spec_passup((int) *arg1, (state_t) arg2, (state_t) arg3);
					break;
				default:
					PANIC(); /* SYSCALL non definita o non implementata */
			}
		} else { /* user mode */
			/* Syscall invocata in user mode, quindi lancio TRAP */
			pgmtrap_handler();
		}
	} else if (CAUSE_EXCCODE_GET(cause) == 9){ /* BREAKPOINT */
		/*
		 * Terminare il processo e la sua progenie
		 * Passare il controllo allo scheduler
		 */
		terminate_process();
		scheduler();
	}
}

/* Program Traps handler */
void pgmtrap_handler(){
	termprint("TRAP\n", 0);
	terminate_process();
	PANIC();
}

/* TLB Management handler */
void tlb_handler(){
	termprint("TLB Exception\n", 0);
	terminate_process();
	PANIC();
}

/* Interrupts handler */
void int_handler(){
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
