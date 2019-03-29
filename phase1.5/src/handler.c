#include "const.h"
#include "types_rikaya.h"
#include "pcb.h"
#include "syscall.h"
#include "interrupt.h"
#include "main.h"

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
	u32* cause = &(caller_process->cause); /* Causa della SYSCALL (tipo di eccezione sollevato) */

	if (CAUSE_EXCCODE_GET(cause) == 8){ /* SYSCALL */
		if ((*status & STATUS_KUp) == 0 ){ /* kernel mode */
			switch (*arg0){
				case SYS1:
					break;
				case SYS2:
					break;
				case TERMINATEPROCESS:
					terminate_process();
					break;
				case SYS4:
					break;
				case SYS5:
					break;
				case SYS6:
					break;
				case SYS7:
					break;
				case SYS8:
					break;
				case SYS9:
					break;
				case SYS10:
					break;
				case SYS11:
					break;
				default:
					PANIC(); /* SYSCALL non definita */
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
	int_old_area = (state_t*) INT_OLDAREA;
	cause = getCAUSE();
	/* Controllo se c'è un pcb attivo sul processore
	 * se true salvo lo stato della CPU nel campo state del pcb */
	if(current_process != NULL) {
		current_process->cpu_slice += (TOD_LO - current_process_tod);	//aggionamento tempo pcb
		current_process->cpu_time += (TOD_LO - current_process_tod);
		save_state(int_old_area, current_process->t_state);   
	}
	switch(int_cause){
		case INT_LOCAL_TIMER:
			int_timer();				//unica interrupt gestita in questa fase
			break;
		case INT_DISK:
			break;
		case INT_TAPE:
			break;
		case INT_PRINTER:
				break;
		case INT_TERMINAL:
				break;
		}
}
