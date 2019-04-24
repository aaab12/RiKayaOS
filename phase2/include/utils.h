#ifndef _UTILS_H
#define _UTILS_H

#include "handler.h"
#include "pcb.h"

extern unsigned int termprint(char *str, unsigned int term);

void initNewAreas();

pcb_t* initPCB(void (*f), int n);

void save_state(state_t* source, state_t* destination);

void aging(struct list_head* head);

void setIT(unsigned int val);

int get_PLT_bit(state_t* state);

#endif
