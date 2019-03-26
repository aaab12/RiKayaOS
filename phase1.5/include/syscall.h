#ifndef __SYSCALL
#define __SYSCALL

/* SYS3: termina il processo corrente e tutta la sua progenie,
 * rimuovendoli dalla ready_queue
 */
void terminate_process();

#endif
