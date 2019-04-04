#include "const.h"
#include <umps/libumps.h>
#include "utils.h"
#include "main.h"

extern void log_process_order(int process);

void scheduler(){
  //termprint("\n==> nello scheduler\n", 1);

  current_process = NULL; /* Setto current_process a NULL per evitare che punti al processo precedentemente caricato quando avvengono interrupt con la ready_queue vuota */
  if (process_counter == 0){
    termprint("HALT\n", 1);

    HALT();
  }
  if (!(emptyProcQ(&ready_queue))){ /* Se la coda dei processi ready non è vuota, manda in esecuzione un processo */
    current_process = removeProcQ(&ready_queue); /* Rimozione del primo elemento di ready_queue */
    current_process->priority = current_process->original_priority; /* Ripristino della priorità originale */
    log_process_order(current_process->original_priority);
		aging(&ready_queue); /* Meccanismo di aging */
    //termprint("==> LDST\n", 1);
    setTIMER(TIME_SLICE); /* Settaggio del time slice */
		LDST(&current_process->p_s); /* Esecuzione del processo tramite il caricamento del suo stato nello stato del processore */
    termprint("QUA?\n", 1);
	} else {
    termprint("WAIT\n", 1);

    WAIT(); /* Il processore va in standby ed aspetta che gli venga segnalato di riprendere l'esecuzione (tramite reset o interrupt) */
  }
  // if(current_process != NULL) {
	//   current_process->cpu_time += (TOD_LO - current_process_tod);
	//   current_process_tod = TOD_LO;
	//   setTIMER(SCHED_TIME_SLICE - current_process->cpu_slice);
  //     LDST(&current_process->p_s);
  // }
}
