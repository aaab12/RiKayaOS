#include "const.h"
#include <umps/libumps.h>
#include "main.h"
#include "pcb.h"
#include "scheduler.h"
#include "utils.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;
extern int process_counter;
extern int clock_semaphore_counter;

void scheduler(){
  current_process = NULL; /* Setto current_process a NULL per evitare che punti al processo precedentemente caricato quando avvengono interrupt con la ready_queue vuota */
  if (process_counter == 0){
    return HALT();
  }
  if (!(emptyProcQ(&ready_queue))){ /* Se la coda dei processi ready non è vuota, manda in esecuzione un processo */
    current_process = removeProcQ(&ready_queue); /* Rimozione del primo elemento di ready_queue */
    current_process->priority = current_process->original_priority; /* Ripristino della priorità originale */
		aging(&ready_queue); /* Meccanismo di aging */
    setTIMER(TIME_SLICE); /* Settaggio del time slice */
    user_mode(current_process); /* Il processo si avvia in user mode */
		LDST(&current_process->p_s); /* Esecuzione del processo tramite il caricamento del suo stato nello stato del processore */
	} else {
    int status = getSTATUS();
    status |= STATUS_IEc ; /* Abilita il bit che controlla gli interrupt */
    status = STATUS_ALL_INT_ENABLE(status); /* Abilita tutti gli interrupt */
    setSTATUS(status);
    WAIT(); /* Il processore va in standby ed aspetta che gli venga segnalato di riprendere l'esecuzione (tramite reset o interrupt) */
  }
}
