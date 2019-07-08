#include "syscall.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;
extern int process_counter;

//void get_cpu_time(unsigned int *user, unsigned int *kernel, unsigned int *wallclock){

//}

//int create_process(state_t *statep, int priority, void ** cpid){

//}

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
