#include "const.h"
#include "types_rikaya.h"
#include "pcb.h"
#include "syscall.h"
#include "interrupt.h"
#include "utils.h"

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
	// u32* arg1 = &(caller_process->reg_a1);
	// u32* arg2 = &(caller_process->reg_a2);
	// u32* arg3	= &(caller_process->reg_a3);

	u32* status = &(caller_process->status); /* Stato del processo al momento della chiamata (kernel/user mode) */
	int  cause =   (caller_process->cause); /* Causa della SYSCALL (tipo di eccezione sollevato) */

	if (CAUSE_EXCCODE_GET(cause) == 8){ /* SYSCALL */
		if ((*status & STATUS_KUp) == 0 ){ /* kernel mode */
			switch (*arg0){
				case TERMINATEPROCESS:
					terminate_process();
					break;
				case SYS1:
				case SYS2:
				case SYS4:
				case SYS5:
				case SYS6:
				case SYS7:
				case SYS8:
				case SYS9:
				case SYS10:
				case SYS11:
				default:
					PANIC(); /* SYSCALL non definita o non implementata */
			}
		} else { /* user mode */
			/*
			 * Lanciare TRAP
			 */
		}
	} else if (CAUSE_EXCCODE_GET(cause) == 9){ /* BREAKPOINT */
		/*
		 * Terminare il processo e la sua progenie
		 * Passare il controllo allo scheduler
		 */
	}
}

/* Program Traps handler */
void pgmtrap_handler(){

}

/* TLB Management handler */
void tlb_handler(){

}

/* Interrupts handler */
void int_handler(){
	/* Linea da cui proviene l'interrupt */
	int line = 0;
	while ((line<10) && !(CAUSE_INT_GET(getCAUSE(), line))) {
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
