#include "const.h"
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

pcb_t* initPCB(void (*f), int n){
  pcb_t* pcb = allocPcb();
  STST(&(pcb->p_s)); /* STST memorizza lo stato corrente del processore nella locazione di memoria fisica fornita  */
  pcb->p_s.pc_epc = (memaddr)(f);
  pcb->p_s.reg_t9 = (memaddr)(f);
  pcb->p_s.reg_sp = RAMTOP-FRAME_SIZE*n;
  pcb->p_s.status = PROCESS_STATUS_1; /* Status con PLT abilitato */
  // pcb->p_s.status = PROCESS_STATUS_2; /* Status con PLT disabilitato */
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

/* Funzione per settare l'Interval Timer */
void setIT(unsigned int val){
  (*((memaddr *)BUS_INTERVALTIMER)) = val;
}

/* Funzione che controlla il 27esimo bit dello status (TE) */
int get_PLT_bit(state_t* state){
  return (state->status & (1U << 27));
}

/******************************************************************************
 * I/O Routines to write on a terminal
 ******************************************************************************/

typedef unsigned int devreg;

#define DEVREGSIZE 16
#define CHAROFFSET 8
#define BUSY 3

/* This function returns the terminal transmitter status value given its address */
devreg termstat(memaddr *stataddr)
{
    return ((*stataddr) & STATUSMASK);
}

/* This function prints a string on specified terminal and returns TRUE if
 * print was successful, FALSE if not   */
unsigned int termprint(char *str, unsigned int term)
{

    memaddr *statusp;
    memaddr *commandp;

    devreg stat;
    devreg cmd;

    unsigned int error = FALSE;

    if (term < DEV_PER_INT)
    {
        /* terminal is correct */
        /* compute device register field addresses */
        statusp = (devreg *)(TERM0ADDR + (term * DEVREGSIZE) + (TRANSTATUS * DEV_REG_LEN));
        commandp = (devreg *)(TERM0ADDR + (term * DEVREGSIZE) + (TRANCOMMAND * DEV_REG_LEN));

        /* test device status */
        stat = termstat(statusp);
        if ((stat == DEV_S_READY) || (stat == DEV_TTRS_S_CHARTRSM))
        {
            /* device is available */

            /* print cycle */
            while ((*str != '\0') && (!error))
            {
                cmd = (*str << CHAROFFSET) | DEV_PRNT_C_PRINTCHR;
                *commandp = cmd;

                /* busy waiting */
                while ((stat = termstat(statusp)) == BUSY)
                    ;

                /* end of wait */
                if (stat != DEV_TTRS_S_CHARTRSM)
                {
                    error = TRUE;
                }
                else
                {
                    /* move to next char */
                    str++;
                }
            }
        }
        else
        {
            /* device is not available */
            error = TRUE;
        }
    }
    else
    {
        /* wrong terminal device number */
        error = TRUE;
    }

    return (!error);
}
