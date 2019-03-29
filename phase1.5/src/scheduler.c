#include "const.h"
#include <umps/libumps.h>
#include "utils.h"
#include "main.h"

extern void log_process_order(int process);

void scheduler(){
  if(current_process != NULL) {
	  current_process->cpu_time += (TOD_LO - current_process_tod);
	  current_process_tod = TOD_LO;
	  setTIMER(SCHED_TIME_SLICE - current_process->cpu_slice);
      LDST(&current_process->p_s);
  }
  else if((current_process = NULL)) { /* Setto current_process a NULL per evitare che punti al processo precedentemente caricato quando avvengono interrupt con la ready_queue vuota */
	if (process_counter == 0)
		HALT();
	if(!(emptyProcQ(&ready_queue))){ /* Se la coda dei processi ready non è vuota, manda in esecuzione un processo */
		current_process = removeProcQ(&ready_queue); /* Rimozione del primo elemento di ready_queue */
		current_process->priority = current_process->original_priority; /* Ripristino della priorità originale */
		setTIMER(TIME_SLICE); /* Settaggio del time slice */
		aging(&ready_queue); /* Meccanismo di aging */
		log_process_order(current_process->original_priority);
		LDST(&current_process->p_s); /* Esecuzione del processo tramite il caricamento del suo stato nello stato del processore */
	} else {
		WAIT(); /* Il processore va in standby ed aspetta che gli venga segnalato di riprendere l'esecuzione (tramite reset o interrupt) */
	}
  }
}
