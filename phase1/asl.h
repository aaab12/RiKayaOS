#ifndef ASL_H
#define ASL_H

#include <types_rikaya.h>

//USER
list_head* list_del_notcirc(list_head *p);
void inizializzasem(struct semd_t* p);
void recursive(pcb_t *root, pcb_t *realroot);
void del_tree(pcb_t* p);

/* ASL handling functions */
semd_t* getSemd(int *key);
void initASL();

int insertBlocked(int *key,pcb_t* p);
pcb_t* removeBlocked(int *key);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int *key);
void outChildBlocked(pcb_t *p);

#endif
