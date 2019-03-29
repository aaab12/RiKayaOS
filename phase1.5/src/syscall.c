#include "syscall.h"
#include "main.h"

/* Recursive Terminate Process */
void r_termine_process(pcb_t* p){
  pcb_t* temp;
  while(temp = removeChild(p)){ /* Estraggo i figli del processo da terminare */
    r_termine_process(temp); /* Richiamo la funzione sul figlio */
  }
  process_counter--;
  outProcQ(ready_queue, p); /* Estraggo il processo dalla coda dei processi ready */
  freePcb(current_process); /* Libero il processo */
}

/* SYS3: termina il processo corrente e tutta la sua progenie,
 * rimuovendoli dalla ready_queue
 */
void terminate_process(){
  r_termine_process(current_process); /* Recursive Terminate Process */
  scheduler();
}
