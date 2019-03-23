#include "const.h"
#include "listx.h"
#include "pcb.h"

/* Funzioni di test */
extern void test1();
extern void test2();
extern void test3();

void initNewAreas(){
  ((state_t *)SYSBK_NEWAREA)->pc_epc = (memaddr)sysbk_handler;
  /* Il general purpose register t9 viene usato per
  migliorare l'efficienza della fase di linking e
  la velocità di esecuzione del codice risultante.
  (pag.72 princOfOperations.pdf)*/
	((state_t *)SYSBK_NEWAREA)->reg_sp = RAMTOP;
  ((state_t *)SYSBK_NEWAREA)->reg_t9 = (memaddr)sysbk_handler;
	((state_t *)SYSBK_NEWAREA)->status = INIT_STATUS_REG;

  ((state_t *)PGMTRAP_NEWAREA)->pc_epc = (memaddr)pgmtrap_handler;
  ((state_t *)PGMTRAP_NEWAREA)->reg_t9 = (memaddr)pgmtrap_handler;
	((state_t *)PGMTRAP_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)PGMTRAP_NEWAREA)->status = INIT_STATUS_REG;

  ((state_t *)TLB_NEWAREA)->pc_epc = (memaddr)tlb_handler;
  ((state_t *)TLB_NEWAREA)->reg_t9 = (memaddr)tlb_handler;
	((state_t *)TLB_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)TLB_NEWAREA)->status = INIT_STATUS_REG;

  ((state_t *)INT_NEWAREA)->pc_epc = (memaddr)int_handler;
  ((state_t *)INT_NEWAREA)->reg_t9 = (memaddr)int_handler;
	((state_t *)INT_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)INT_NEWAREA)->status = INIT_STATUS_REG;
}

int main() {
  initNewAreas(); /* Iniziliazzazione delle new area */
  initPcbs(); /* Inizializzazione lista PCB liberi */
  LIST_HEAD(ready_queue); /* Inizializzazione lista processi liberi */

  return 0;
}
