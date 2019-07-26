#ifndef _PCB_H
#define _PCB_H

#include "const_rikaya.h"
#include "types_rikaya.h"
#include "listx.h"

//USER
void inizializzapcb(struct pcb_t* p);
void initPcbState(state_t* state);

/* PCB handling functions */

/* PCB free list handling functions */
void initPcbs(void);
void freePcb(pcb_t *p);
pcb_t *allocPcb(void);

/* PCB queue handling functions */
void mkEmptyProcQ(struct list_head *head);
int emptyProcQ(struct list_head *head);
void insertProcQ(struct list_head *head, pcb_t *p);
pcb_t *headProcQ(struct list_head *head);

pcb_t *removeProcQ(struct list_head *head);
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

/* Tree view functions */
int emptyChild(pcb_t *this);
void insertChild(pcb_t *prnt, pcb_t *p);
pcb_t *removeChild(pcb_t *p);
pcb_t *outChild(pcb_t *p);

#endif
