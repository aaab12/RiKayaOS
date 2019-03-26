#include "const.h"

void aging(struct list_head* head){
  pcb_t* p = headProcQ(&head); /* Puntatore al primo elemento della lista */
  while (p != NULL) {
    p->priority += 1; /* Meccanismo di aging */
    p = p->p_next;
  }
}

void scheduler(){
  current_process = NULL; /* Setto current_process a NULL per evitare che punti al processo precedentemente caricato quando avvengono interrupt con la ready_queue vuota */
  if(!(emptyProcQ(&ready_queue))){ /* Se la coda dei processi ready non è vuota, manda in esecuzione un processo */
    current_process = removeProcQ(&ready_queue); /* Rimozione del primo elemento di ready_queue */
    current_process->priority = current_process->original_priority; /* Ripristino della priorità originale */
    setTIMER(TIME_SLICE); /* Settaggio del time slice */
    aging(&ready_queue); /* Meccanismo di aging */
    LDST(&current_process->p_s); /* Esecuzione del processo tramite il caricamento del suo stato nello stato del processore */
  }
  WAIT(); /* Il processore va in standby ed aspetta che gli venga segnalato di riprendere l'esecuzione (tramite reset o interrupt) */
}
