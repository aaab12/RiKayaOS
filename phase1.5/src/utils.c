#include "utils.h"

void save_state(state_t* source, state_t* destination) {
	destination->entry_hi = source->entry_hi;
	destination->cause = source->cause;
	destination->status = source->status;
	destination->pc_epc = source->pc_epc;
	destination->hi = source->hi;
	destination->lo = source->lo;
	int i;
	for(i = 0; i <= 29; i++) {
		destination->gpr[i] = source->gpr[i];
	}
}


void aging(struct list_head* head){
  pcb_t* p = headProcQ(head); /* Puntatore al primo elemento della lista */
  while (p != NULL) {
    p->priority += 1; /* Meccanismo di aging */
    p = p->p_next.next;
  }
}
