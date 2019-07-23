#include "syscall.h"

extern struct list_head ready_queue;
extern pcb_t* current_process;
extern int process_counter;
extern int clock_semaphore;
extern int clock_semaphore_counter;

/* SYS1: ritorna il tempo passato in user mode, kernel mode e tempo totale trascorso dalla prima attivazione */
void get_cpu_time(unsigned int *user, unsigned int *kernel, unsigned int *wallclock){
  user_mode(current_process); /* Il processo torna in user mode (ed aggiorna la quantita di tempo trascorsa in kernel mode) */

  if(user) *user = current_process->user_time_total;
  if(kernel) *kernel = current_process->kernel_time_total;
  if(wallclock) *wallclock = (TOD_LO - current_process->wallclock_time_start);
}

/* SYS2: crea un processo figlio del processo corrente */
int create_process(state_t *statep, int priority, void ** cpid){
  pcb_t *new_process = allocPcb(); /* Rimuove un PCB dalla pcbFree */

  if (cpid) *((pcb_t **)cpid) = new_process; /* Se cpid != NULL, *cpid contiene indirizzo del processo figlio */

  if (!new_process) return -1; /* Se la pcbFree è vuota, ritorna -1 */

  new_process->priority = priority;
  new_process->original_priority = priority;

  save_state(statep, &(new_process->p_s)); /* Lo stato del figlio è uguale allo stato del padre */

  new_process->wallclock_time_start = TOD_LO; /* Il processo nasce al tempo TOD_LO */

  insertChild(current_process, new_process); /* Inserisce il nuovo processo come ultimo figlio del processo corrente */
  insertProcQ(&ready_queue, new_process); /* Inserisce il nuovo processo nella coda dei processi in stato ready */

  return 0;
}

/* SYS3: */
int terminate_process(void ** pid){
  pcb_t *pcb;
  pcb_t *parent = NULL;
  pcb_t *tutor = NULL;

  if(pid) pcb = *((pcb_t **)pid);
  else pcb = current_process;

  if (!pcb->p_parent) return -1; /* Se il processo è il root process, errore */

  if(pcb != current_process){ /* Il processo da terminare deve essere il corrente o discendente del corrente */
    parent = pcb;
    while((parent = parent->p_parent))
      if(current_process == parent) break;
    return -1; /* Se non è discendente del processo corrente, errore */
  }

  tutor = pcb;
  while(!(tutor = tutor->p_parent)->tutor); /* Trova il primo antenato tutor */

  if(pcb->p_semkey){ /* Se il processo è bloccato su un semaforo, rilascio la risorsa */
    *pcb->p_semkey += 1;
  }

  while(!emptyChild(pcb)) insertChild(tutor, removeChild(pcb)); /* Se il processo da terminare ha figli, diventano figli del tutor */
  outChild(pcb); /* Rimuove il processo dalla lista dei figli di suo padre */
  outBlocked(pcb); /* Rimuove il processo dal semaforo su cui è eventualmente bloccato */
  outProcQ(&ready_queue, pcb); /* Rimuove il processo dalla ready_queue */
  freePcb(pcb); /* Libero il PCB */

  if(pcb == current_process){ /* Se termina il processo attuale, il controllo passa allo scheduler */
    current_process = NULL;
    scheduler();
  }

  return 0;
}

/* SYS4: rilascio di un semaforo */
void verhogen(int *semaddr){
  pcb_t *pcb;

  *semaddr += 1; /* Aumenta il valore del semaforo */

  if(*semaddr <= 0){ /* Se il semaforo è negativo, significa che c'è almeno un processo in attesa su quel semaforo */
    pcb = removeBlocked(semaddr); /* Recupero del primo processo bloccato sul semaforo */
    if(pcb){ /* Se la key del semaforo è presente */
      pcb->priority = pcb->original_priority; /* Ripristino della priorità originale */
      insertProcQ(&ready_queue, pcb); /* Inserisce il processo sbloccato nella coda dei processi in stato ready */
    }
  }
}

/* SYS5: richiesta di un semaforo */
void passeren(int *semaddr){
  *semaddr -= 1; /* Diminuisce il valore del semaforo */
  if(*semaddr < 0){ /* Se il valore del semaforo è negativo */
    outProcQ(&ready_queue, current_process); /* Rimuove il processo dalla ready queue */
    insertBlocked(semaddr, current_process); /* Blocca il processo sul semaforo */
    save_state((state_t *)SYSBK_OLDAREA, &current_process->p_s); /* Ripristino lo stato originario del processo */
    user_mode(current_process); /* Il processo torna in user mode */
    scheduler(); /* Il controllo passa allo scheduler */
  }
}

/* SYS6: sospende il processo per 100ms */
void wait_clock(){
  passeren(&clock_semaphore); /* P sul semaforo del clock */
  clock_semaphore_counter++;
}

int do_io(unsigned int command, unsigned int *reg, unsigned int rw){
  int first_terminal = DEV_REG_ADDR(INT_TERMINAL, 0); /* Indirizzo del primo terminale */

  if((unsigned int)reg < first_terminal){ /* Il device non è un terminale */


  } else { /* Il device è un terminale */
    ((termreg_t *)reg)->transm_command = command; /* Imposta il carattere da trasmettere e fa partire l'operazione di stampa su terminale */

    while (((termreg_t *)reg)->transm_status == 3) termprint("busy\n", 1); /* Aspetta finchè lo stato del terminale non è più "BUSY" */

    return *(reg + 2); /* (base) + 2 = TRANSM_STATUS */
  }
}

/* SYS8: imposta il processo corrente come tutor */
void set_tutor(){
  current_process->tutor = TRUE;
}

int spec_passup(int type, state_t *old, state_t *new){
  if(current_process->passup[type]) return -1;
  current_process->passup[type] = TRUE;
  current_process->passup_oldarea[type] = old;
  current_process->passup_newarea[type] = new;

  return 0;
}
