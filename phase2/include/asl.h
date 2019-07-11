#ifndef ASL_H
#define ASL_H

#include "types_rikaya.h"

//USER
void delTree(pcb_t* p);

/* ASL handling functions */
semd_t* getSemd(int *key);
void initAsl();

int insertBlocked(int *key,pcb_t* p);
pcb_t* removeBlocked(int *key);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int *key);
void outChildBlocked(pcb_t *p);

#endif
