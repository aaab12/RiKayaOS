#ifndef _UTILS_H
#define _UTILS_H

#include "const.h"
#include "handler.h"
#include "pcb.h"

void initNewAreas();

pcb_t* initPCB(void (*f), int n);

void save_state(state_t* source, state_t* destination);

void aging(struct list_head* head);

void setIT(unsigned int val);

void kernel_mode(pcb_t *process);

void user_mode(pcb_t *process);

int device_number(memaddr* bitmap);

/******************************************************************************/

unsigned int termstat(memaddr *stataddr);

unsigned int termprint(char *str, unsigned int term);

#endif
