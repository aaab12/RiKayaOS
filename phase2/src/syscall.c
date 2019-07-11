#include "syscall.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;
extern int process_counter;

void get_cpu_time(unsigned int *user, unsigned int *kernel, unsigned int *wallclock){
  user_mode(current_process); /* Il processo torna in user mode (ed aggiorna la quantita di tempo trascorsa in kernel mode) */

  if(user) *user = current_process->user_time_total;
  if(kernel) *kernel = current_process->kernel_time_total;
  if(wallclock) *wallclock = (TOD_LO - current_process->wallclock_time_start);
}

int create_process(state_t *statep, int priority, void ** cpid){
  pcb_t *new_process = allocPcb(); /* Rimuove un PCB dalla pcbFree */

  if (cpid) *((pcb_t **)cpid) = new_process; /* Se cpid != NULL, *cpid contiene indirizzo del processo figlio */

  if (!new_process) return -1; /* Se la pcbFree è vuota, ritorna -1 */

  new_process->priority = priority;
  new_process->original_priority = priority;

  save_state(statep, &(new_process->p_s)); /* Lo stato del figlio è uguale allo stato del padre */

  new_process->wallclock_time_start = TOD_LO; /* Il processo nasce al tempo TOD_LO */

  insertChild(current_process, new_process); /* Inserisce il nuovo processo come ultimo figlio del processo corrente */
  insertProcQ(&ready_queue, new_process); /* Inserisce il nuovo processo nella coda dei processi in stato ready */

  return 0;
}

//void verhogen(int *semaddr){

//}

//void passeren(int *semaddr){

//}

//void wait_clock(){

//}

//int do_io(unsigned int command, unsigned int *register){

//}

//void set_tutor(){

//}

//int spec_passup(int type, state_t *old, state_t *new){

//}

/* Recursive Terminate Process */
void r_termine_process(pcb_t* p){
  pcb_t* temp;
  while((temp = removeChild(p))){ /* Estraggo i figli del processo da terminare */
    r_termine_process(temp); /* Richiamo la funzione sul figlio */
  }
  process_counter--;
  outProcQ(&ready_queue, p); /* Estraggo il processo dalla coda dei processi ready */
  freePcb(p); /* Libero il processo */
}

/* SYS3: termina il processo corrente e tutta la sua progenie,
 * rimuovendoli dalla ready_queue
 */
void terminate_process(){
  r_termine_process(current_process); /* Recursive Terminate Process */
  scheduler();
}
