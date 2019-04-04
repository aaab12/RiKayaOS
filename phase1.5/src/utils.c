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
	struct list_head* scan;
	list_for_each(scan, head){
		container_of(scan, pcb_t, p_next)->priority++;			/* Meccanismo di aging */
  }
}
