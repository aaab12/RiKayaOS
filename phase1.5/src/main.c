#include "const.h"
#include "listx.h"
#include "pcb.h"
#include "scheduler.h"
#include <umps/libumps.h>


int main() {
  initNewAreas(); /* Iniziliazzazione delle new area */
  initPcbs(); /* Inizializzazione lista PCB liberi */
  mkEmptyProcQ(&ready_queue); /* Inizializzazione lista processi in stato ready */
  
  int process_counter = 0;
  int current_process_tod = 0;
  current_process = NULL;
  		
  pcb_t* pcb1 = initPCB(test1, 1); /* Inizializza primo processo */
  pcb_t* pcb2 = initPCB(test2, 2); /* Inizializza secondo processo */
  pcb_t* pcb3 = initPCB(test3, 3); /* Inizializza terzo processo */
	
  insertProcQ(&ready_queue, pcb1); /* Inserimento di pcb1 nella coda dei processi in stato ready */
  insertProcQ(&ready_queue, pcb2); /* Inserimento di pcb2 nella coda dei processi in stato ready */
  insertProcQ(&ready_queue, pcb3); /* Inserimento di pcb3 nella coda dei processi in stato ready */

  scheduler(); /* Passaggio del controllo allo scheduler */
  return 0;
}

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
  pcb->p_s.pc_epc = (memaddr)(*f);
  pcb->p_s.reg_t9 = (memaddr)(*f);
  pcb->p_s.reg_sp = RAMTOP-FRAMESIZE*n;
  pcb->p_s.status = PROCESS_STATUS;
  pcb->priority = n;
  pcb->original_priority = n;
  process_counter++;
  return pcb;
}
