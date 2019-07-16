#include "main.h"

struct list_head ready_queue; /* Coda dei processi in stato ready */
pcb_t* current_process; /* Puntatore al processo attualmente in esecuzione */
int process_counter;	/* Contatore dei processi */
int clock_semaphore; /* Semaforo del clock (SYS6) */
int clock_semaphore_counter; /* Contatore dei processi bloccati sul semaforo del clock */
int device_semaphore[DEV_PER_INT*(DEV_USED_INTS-1)]; /* Semafori dei device: numero di device per interrupt line * numero di interrupt line dedicate ai device (meno una perchè l'ultima interrupt line è per i terminali) */
int terminal_semaphore[DEV_PER_INT][2]; /* Semafori dei terminali: numero di device (8) * 2 (read e write) */

int main() {
  initNewAreas(); /* Iniziliazzazione delle new area */
  initPcbs(); /* Inizializzazione lista PCB liberi */
  initAsl(); /* Inizializzazione lista SEMD liberi */
  mkEmptyProcQ(&ready_queue); /* Inizializzazione lista processi in stato ready */

  process_counter = 0;
  current_process = NULL;

  /* Inizializzazione semafori */
  clock_semaphore = 0;
  clock_semaphore_counter = 0;
  int i;
  for(i=0; i<(DEV_PER_INT*(DEV_USED_INTS-1)); i++)
    device_semaphore[i] = 0;
  for(i=0; i<DEV_PER_INT; i++)
  {
    terminal_semaphore[i][0] = 0;
    terminal_semaphore[i][1] = 0;
  }

  // Per ora settiamo priorità 1
  pcb_t* pcb = initPCB(test, 1); /* Inizializza processo */

  insertProcQ(&ready_queue, pcb); /* Inserimento di pcb nella coda dei processi in stato ready */

  scheduler(); /* Passaggio del controllo allo scheduler */
  return 0;
}
