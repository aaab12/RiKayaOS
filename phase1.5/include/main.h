#ifndef _MAIN_H
#define _MAIN_H

#include "const.h"
#include "pcb.h"
#include "scheduler.h"
#include "utils.h"

/* Funzioni di test */
extern void test1();
extern void test2();
extern void test3();

struct list_head ready_queue; /* Coda dei processi in stato ready */
pcb_t* current_process; /* Puntatore al processo attualmente in esecuzione */
int process_counter;	/* Contatore dei processi */

#endif
