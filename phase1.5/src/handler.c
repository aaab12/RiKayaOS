#include "const.h"
#include "types_rikaya.h"
#include "pcb.h"
#include "syscall.h"
#include "interrupt.h"
#include "main.h"
#include "utils.h"

/* SYSCALL/Breakpoint handler */
void sysbk_handler(){
	termprint("SYSBK HANDLER\n", 1);
  /* Processo chiamante */
	state_t* caller_process = (state_t *)SYSBK_OLDAREA;

  /* Il Program Counter del processo chiamante deve essere incrementato di una WORD_SIZE
   * per evitare loop, in quanto le SYSCALL che non comportano la terminazione di un processo
   * fanno tornare il controllo al flusso di esecuzione che ha richiesto la SYSCALL stessa.
   */
	caller_process->pc_epc += WORDSIZE;

	/* Parametri della SYSCALL */
	u32* arg0 = &(caller_process->reg_a0); /* Numero della SYSCALL */
//	u32* arg1 = &(caller_process->reg_a1);
//	u32* arg2 = &(caller_process->reg_a2);
//	u32* arg3	= &(caller_process->reg_a3);

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
	//termprint("pgmtrap_handler\n", 1);
}

/* TLB Management handler */
void tlb_handler(){
	termprint("tlb_handler\n", 1);
}

/* Interrupts handler */
void int_handler(){
	//termprint("\n==> nell'interrupt handler\n", 1);
	/* Processo interrotto (non necessariamente quello che ha sollevato l'interrupt) */
	state_t* caller_process = (state_t *)INT_OLDAREA;
	/* Linea da cui proviene l'interrupt */
	int line = 0;
	while ((line<10) && !(CAUSE_INT_GET(getCAUSE(), line))) {
		line++;
	}
	switch(line){
		case INT_PLT:
		//termprint("==> plt interrupt\n", 1);
			plt_handler();
			break;
		case INT_PROCESSOR:
			termprint("processor\n", 1);
		case INT_TIMER:
			termprint("timer\n", 1);
		case INT_DISK:
			termprint("disk\n", 1);
		case INT_TAPE:
			termprint("tape\n", 1);
		case INT_NETWORK:
			termprint("network\n", 1);
		case INT_PRINTER:
			termprint("printer\n", 1);
		case INT_TERMINAL:
			termprint("==> terminal interrupt\n", 1);
			break;
		default:
			//termprint("PANIC\n", 1);
			PANIC(); /* Interrupt non implementato o linea errata */
	}
}
