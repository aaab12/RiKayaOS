#ifndef MAIN_H
#define MAIN_H

#include "types_rikaya.h"

/* Funzioni di test */
extern void test1();
extern void test2();
extern void test3();

struct list_head ready_queue; /* Coda dei processi in stato ready */
pcb_t* current_process; /* Puntatore al processo attualmente in esecuzione */
int process_counter;	/* Contatore dei processi */
int current_process_tod;

void initNewAreas();
pcb_t* initPCB(void (*f), int n);

#endif
