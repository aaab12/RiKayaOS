#include "handler.h"

/* SYSCALL/Breakpoint handler */
void sysbk_handler(){
  /* Processo chiamante */
	state_t* caller_process = (state_t *)SYSBK_OLDAREA;
  /* Il Program Counter del processo chiamante deve essere incrementato di una WORD_SIZE
   * per evitare loop, in quanto le SYSCALL che non comportano la terminazione di un processo
   * fanno tornare il controllo al flusso di esecuzione che ha richiesto la SYSCALL stessa.
   */
	caller_process->pc_epc += WORDSIZE;

	/* Parametri della SYSCALL */
	u32* arg0 = &(caller_process->reg_a0); /* Numero della SYSCALL */
	u32* arg1 = &(caller_process->reg_a1);
	u32* arg2 = &(caller_process->reg_a2);
	u32* arg3	= &(caller_process->reg_a3);

	u32* status = &(caller_process->status); /* Stato del processo al momento della chiamata (kernel/user mode) */
	int  cause =   (caller_process->cause); /* Causa della SYSCALL (tipo di eccezione sollevato) */

	if (CAUSE_EXCCODE_GET(cause) == 8){ /* SYSCALL */
		if ((*status & STATUS_KUp) == 0 ){ /* kernel mode */
			switch (*arg0){
				case GETCPUTIME:
					get_cpu_time();
					break;
				case CREATEPROCESS:
					create_process();
					break;
				case TERMINATEPROCESS:
					terminate_process();
					break;
				case VERHOGEN:
					verhogen();
					break;
				case PASSEREN:
					passeren();
					break;
				case WAITCLOCK:
					wait_clock();
					break;
				case IOCOMMAND:
					do_io();
					break;
				case SETTUTOR:
					set_tutor();
					break;
				case SPECPASSUP:
					spec_passup();
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
	int line = 0;
	while ((line<7) && !(CAUSE_INT_GET(getCAUSE(), line))) {
		line++;
	}
	switch(line){
		case INT_PLT:
			plt_handler(); /* Process Local Timer handler */
			break;
		case INT_TIMER:
			it_handler(); /* Interval Timer handler */
			break;
		case INT_PROCESSOR:
		case INT_DISK:
		case INT_TAPE:
		case INT_NETWORK:
		case INT_PRINTER:
		case INT_TERMINAL:
		default:
			PANIC(); /* Interrupt non implementato o linea errata */
	}
}
