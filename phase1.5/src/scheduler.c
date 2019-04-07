#include "scheduler.h"

extern void log_process_order(int process);

void scheduler(){
  current_process = NULL; /* Setto current_process a NULL per evitare che punti al processo precedentemente caricato quando avvengono interrupt con la ready_queue vuota */
  if (process_counter == 0){
    return HALT();
  }
  if (!(emptyProcQ(&ready_queue))){ /* Se la coda dei processi ready non è vuota, manda in esecuzione un processo */
    current_process = removeProcQ(&ready_queue); /* Rimozione del primo elemento di ready_queue */
    current_process->priority = current_process->original_priority; /* Ripristino della priorità originale */
    log_process_order(current_process->original_priority); /* Log dei context switch */
		aging(&ready_queue); /* Meccanismo di aging */
    if (get_PLT_bit(&current_process->p_s)) /* In base al valore del bit TE, si utilizza PLT oppure Interval Timer */
    {
      setTIMER(TIME_SLICE); /* Settaggio del time slice */
    } else {
      setIT(TIME_SLICE); /* Settaggio del time slice */
    }
		LDST(&current_process->p_s); /* Esecuzione del processo tramite il caricamento del suo stato nello stato del processore */
	} else {
    WAIT(); /* Il processore va in standby ed aspetta che gli venga segnalato di riprendere l'esecuzione (tramite reset o interrupt) */
  }
}
