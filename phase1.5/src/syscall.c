#include "pcb.h"

/* SYS3: termina il processo corrente e tutta la sua progenie,
 * rimuovendoli dalla ready_queue
 */
void terminate_process(){
  pcb_t* temp1;
  while(temp1 = removeChild(current_process)){ /* Estraggo i figli del processo da terminare */
    pcb_t* temp2;
    temp2 = outProcQ(ready_queue, temp1); /* Estraggo il figlio dalla coda dei processi ready */
    if(temp2) freePcb(temp2); /* Libero il figlio */
  }
  freePcb(current_process); /* Libero il processo da terminare */
  scheduler();
}
