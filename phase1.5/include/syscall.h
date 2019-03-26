#ifndef __SYSCALL
#define __SYSCALL

/* Recursive Terminate Process */
void r_termine_process(pcb_t* p);

/* SYS3: termina il processo corrente e tutta la sua progenie,
 * rimuovendoli dalla ready_queue
 */
void terminate_process();

#endif
