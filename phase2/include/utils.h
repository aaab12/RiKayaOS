#ifndef _UTILS_H
#define _UTILS_H

void initNewAreas();

pcb_t* initPCB(void (*f), int n);

void save_state(state_t* source, state_t* destination);

void aging(struct list_head* head);

void setIT(unsigned int val);

void kernel_mode(pcb_t *process);

void user_mode(pcb_t *process);

#endif
