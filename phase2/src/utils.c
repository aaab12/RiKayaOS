#include "utils.h"

extern int process_counter;

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

pcb_t* initPCB(void (*f), int priority){
  pcb_t* pcb = allocPcb();
  STST(&(pcb->p_s)); /* STST memorizza lo stato corrente del processore nella locazione di memoria fisica fornita  */
  pcb->p_s.pc_epc = (memaddr)(f);
  pcb->p_s.reg_t9 = (memaddr)(f);
  pcb->p_s.reg_sp = RAMTOP-FRAME_SIZE;
  pcb->p_s.status = PROCESS_STATUS; /* Status con PLT abilitato */
  pcb->priority = priority;
  pcb->original_priority = priority;
  pcb->wallclock_time_start = TOD_LO; /* Il processo nasce al tempo TOD_LO */
  pcb->tutor = TRUE; /* Il processo principale è tutor */
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
	for(i = 0; i <= 29; i++){
		destination->gpr[i] = source->gpr[i];
	}
}

void aging(struct list_head* head){
	struct list_head* scan;
	list_for_each(scan, head){
		container_of(scan, pcb_t, p_next)->priority++;			/* Meccanismo di aging */
  }
}

/* Funzione per settare l'Interval Timer */
void setIT(unsigned int val){
  (*((memaddr *)BUS_INTERVALTIMER)) = val;
}

/* Funzione per calcolare il tempo passato in user mode prima di passare a kernel mode */
void kernel_mode(pcb_t *process){
  if(process->user_time_start){ /* Se il processo era in user mode */
    process->user_time_total += (TOD_LO - process->user_time_start); /* Tempo totale in cui è rimasto in esecuzione in user mode */
    process->user_time_start = 0; /* Il processo esce dalla user mode */
  }
  process->kernel_time_start = TOD_LO; /* Il processo entra in kernel mode al tempo TOD_LO */
}

/* Funzione per calcolare il tempo passato in kernel mode prima di passare a user mode */
void user_mode(pcb_t *process){
  if(process->kernel_time_start){ /* Se il processo era in kernel mode */
    process->kernel_time_total += (TOD_LO - process->kernel_time_start); /* Tempo totale in cui è rimasto in esecuzione in kernel mode */
    process->kernel_time_start = 0; /* Il processo esce dalla kernel mode */
  }
  process->user_time_start = TOD_LO; /* Il processo entra in user mode al tempo TOD_LO */
}

int device_number(memaddr* bitmap) {
  int device_no = 0;
  while(*bitmap > 1){
    device_no++;
    *bitmap >>= 1;
  }
  return device_no;
}

pcb_t* V_pcb(int *semaddr){
  pcb_t* pcb;

  *semaddr += 1; /* Aumenta il valore del semaforo */

  if(*semaddr <= 0){ /* Se il semaforo è negativo, significa che c'è almeno un processo in attesa su quel semaforo */
    pcb = removeBlocked(semaddr); /* Recupero del primo processo bloccato sul semaforo */
    if(pcb) /* Se la key del semaforo è presente */
      pcb->priority = pcb->original_priority; /* Ripristino della priorità originale */
    return pcb;
  }
  return NULL;
}

/******************************************************************************/

unsigned int termstat(memaddr *stataddr)
{
    return ((*stataddr) & STATUSMASK);
}

unsigned int termprint(char *str, unsigned int term)
{
    memaddr *statusp;
    memaddr *commandp;
    unsigned int stat;
    unsigned int cmd;
    unsigned int error = FALSE;
    if (term < 8)
    {
        statusp = (unsigned int *)(0x10000250 + (term * 16) + (2 * 4));
        commandp = (unsigned int *)(0x10000250 + (term * 16) + (3 * 4));
        stat = termstat(statusp);
        if ((stat == 1) || (stat == 5))
        {
            while ((*str != '\0') && (!error))
            {
                cmd = (*str << 8) | 2;
                *commandp = cmd;
                while ((stat = termstat(statusp)) == 3)
                    ;
                if (stat != 5)
                {
                    error = TRUE;
                }
                else
                {
                    str++;
                }
            }
        }
        else
        {
            error = TRUE;
        }
    }
    else
    {
        error = TRUE;
    }

    return (!error);
}
