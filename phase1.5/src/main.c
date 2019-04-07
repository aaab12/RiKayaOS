#include "main.h"

int main() {
  initNewAreas(); /* Iniziliazzazione delle new area */
  initPcbs(); /* Inizializzazione lista PCB liberi */
  mkEmptyProcQ(&ready_queue); /* Inizializzazione lista processi in stato ready */

  process_counter = 0;
  current_process = NULL;

  pcb_t* pcb1 = initPCB(test1, 1); /* Inizializza primo processo */
  pcb_t* pcb2 = initPCB(test2, 2); /* Inizializza secondo processo */
  pcb_t* pcb3 = initPCB(test3, 3); /* Inizializza terzo processo */

  insertProcQ(&ready_queue, pcb1); /* Inserimento di pcb1 nella coda dei processi in stato ready */
  insertProcQ(&ready_queue, pcb2); /* Inserimento di pcb2 nella coda dei processi in stato ready */
  insertProcQ(&ready_queue, pcb3); /* Inserimento di pcb3 nella coda dei processi in stato ready */

  scheduler(); /* Passaggio del controllo allo scheduler */
  return 0;
}
