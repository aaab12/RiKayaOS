/* SYSCALL/Breakpoint handler */
void sysbk_handler(){
  /* Recupero il processo chiamante */
	state_t* caller_process = (state_t *)SYSBK_OLDAREA;
  /* Il Program Counter del processo chiamante deve essere incrementato di una WORD_SIZE
   * per evitare loop, in quanto le SYSCALL che non comportano la terminazione di un processo
   * fanno tornare il controllo al flusso di esecuzione che ha richiesto la SYSCALL stessa.
   */
	caller_process->pc_epc += WORDSIZE;

}

/* Program Traps handler */
void pgmtrap_handler(){

}

/* TLB Management handler */
void tlb_handler(){

}

/* Interrupts handler */
void int_handler(){

}
