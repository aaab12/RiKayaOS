#include "main.h"

struct list_head ready_queue; /* Coda dei processi in stato ready */
pcb_t* current_process; /* Puntatore al processo attualmente in esecuzione */
int process_counter;	/* Contatore dei processi */

int main() {
  initNewAreas(); /* Iniziliazzazione delle new area */
  initPcbs(); /* Inizializzazione lista PCB liberi */
  initASL(); /* Inizializzazione lista SEMD liberi */
  mkEmptyProcQ(&ready_queue); /* Inizializzazione lista processi in stato ready */

  process_counter = 0;
  current_process = NULL;

  // Per ora settiamo priorità 1
  pcb_t* pcb = initPCB(test, 1); /* Inizializza processo */

  insertProcQ(&ready_queue, pcb); /* Inserimento di pcb nella coda dei processi in stato ready */

  scheduler(); /* Passaggio del controllo allo scheduler */
  return 0;
}
