#include "utils.h"
#include "const.h"
#include "handler.h"
#include "main.h"
#include <umps/libumps.h>

void initNewAreas(){
  ((state_t *)SYSBK_NEWAREA)->pc_epc = (memaddr)sysbk_handler;
  /* Il general purpose register t9 viene usato per
   * migliorare l'efficienza della fase di linking e
   * la velocità di esecuzione del codice risultante.
   * (pag.72 princOfOperations.pdf)
   */
  ((state_t *)SYSBK_NEWAREA)->reg_t9 = (memaddr)sysbk_handler;
	((state_t *)SYSBK_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)SYSBK_NEWAREA)->status = EXCEPTION_STATUS;

  ((state_t *)PGMTRAP_NEWAREA)->pc_epc = (memaddr)pgmtrap_handler;
  ((state_t *)PGMTRAP_NEWAREA)->reg_t9 = (memaddr)pgmtrap_handler;
	((state_t *)PGMTRAP_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)PGMTRAP_NEWAREA)->status = EXCEPTION_STATUS;

  ((state_t *)TLB_NEWAREA)->pc_epc = (memaddr)tlb_handler;
  ((state_t *)TLB_NEWAREA)->reg_t9 = (memaddr)tlb_handler;
	((state_t *)TLB_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)TLB_NEWAREA)->status = EXCEPTION_STATUS;

  ((state_t *)INT_NEWAREA)->pc_epc = (memaddr)int_handler;
  ((state_t *)INT_NEWAREA)->reg_t9 = (memaddr)int_handler;
	((state_t *)INT_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)INT_NEWAREA)->status = EXCEPTION_STATUS;
}

pcb_t* initPCB(void (*f), int n){
  pcb_t* pcb = allocPcb();
  STST(&(pcb->p_s)); /* STST memorizza lo stato corrente del processore nella locazione di memoria fisica fornita  */
  pcb->p_s.pc_epc = (memaddr)(f);
  pcb->p_s.reg_t9 = (memaddr)(f);
  pcb->p_s.reg_sp = RAMTOP-FRAMESIZE*n;
  pcb->p_s.status = PROCESS_STATUS;
  pcb->priority = n;
  pcb->original_priority = n;
  process_counter++;
  return pcb;
}

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
