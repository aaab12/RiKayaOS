#include "pcb.h"

// pcbFree_table: array dei pcb free
HIDDEN struct pcb_t pcbFree_table[MAXPROC];
// pcbfree_h: sentinella pcbFree
HIDDEN struct list_head pcbfree_h;
// pcbFree: puntatore alla lista dei pcb free
HIDDEN struct list_head* pcbFree = &pcbfree_h;

/* PCB HANDLING FUNCTIONS */

/* PCB free list handling functions */

/* Inizializzazione della lista dei PCB liberi in modo da contenere tutti gli
 * elementi della pcbFree_table */
void initPcbs() {
	INIT_LIST_HEAD(pcbFree);
	int i;
	for(i = 0; i < MAXPROC; i++){
		struct list_head* ptr = &pcbFree_table[i].p_next;
		list_add_tail(ptr, pcbFree);
  }
}

/* Inserisce il PCB puntato da p nella lista dei PCB liberi */
void freePcb(pcb_t * p) {
	list_head *scan = pcbFree->next;					// controllo che il PCB p non sia già nella lista pcbFree
	while(scan != pcbFree) {
		if(container_of(scan, pcb_t, p_next) == p) {	// p è già nella lista pcbFree, quindi non devo aggiungerlo
			return;
		} else {
			scan = scan->next;
		}
	}
	list_add_tail(&p->p_next, pcbFree);
}

/* Rimuove un PCB della pcbFree, inizializza i suoi campi e lo restituisce
 * Restituisce NULL se la pcbFree è vuota */
pcb_t* allocPcb() {
	if(list_empty(pcbFree)) {
		return NULL;
	} else {															// ptrp: ultimo elemento della pcbFree
		struct list_head* ptr = pcbFree->prev;
		struct pcb_t * ptrp = container_of(ptr, pcb_t, p_next);
		list_del(&ptrp->p_next);
		inizializzapcb(ptrp);
		return ptrp;
	}
}

/* PCB QUEUE HANDLING FUNCTIONS */

/* Inizializza la lista dei PCB istanziando la sentinella */
void mkEmptyProcQ(struct list_head *head) {
	INIT_LIST_HEAD(head);
}

/* Lista vuota? */
int emptyProcQ(struct list_head *head) {
  return list_empty(head);
}

/* Inserisce il PCB puntato da p nella lista puntata da head in
 * base alla priorità del processo: in testa priorità più alta */
void insertProcQ(struct list_head *head, pcb_t *p) {
	if(emptyProcQ(head) || headProcQ(head)->priority < p->priority)		//inserisco il pcb in una lista vuota o in testa
		list_add(&p->p_next, head);										//a una lista di pcb tutti con priorità minore
	else {							//itera dal fondo della lista per trovare la sua collocazione in base alla priorità
		struct list_head* scan;
		list_for_each_prev(scan, head) {
			if((container_of(scan, pcb_t, p_next))->priority >= p->priority) {
				__list_add(&p->p_next, scan, scan->next);
				return;
			}
		}
	}
}

/* Restituisce il PCB in testa alla lista puntata da head
 * senza rimuoverlo. NULL se lista vuota */
pcb_t *headProcQ(struct list_head *head) {
  if(emptyProcQ(head))
    return NULL;
  return container_of(head->next, pcb_t, p_next);
}

/* Inizializza la lista dei PCB istanziando la sentinella */
pcb_t *removeProcQ(struct list_head *head) {
	if(emptyProcQ(head))
		return NULL;
  	pcb_t* p = headProcQ(head);		//puntatore al primo pcb della lista
    list_del(&p->p_next);			//eliminazione del pcb dalla lista
    return p;						//restituzione dello stesso
}

/* Rimuove il PCB puntato da p dalla coda puntata da head
 * NULL se lista vuota. p in posizione qualunque nella lista */
pcb_t *outProcQ(struct list_head *head, pcb_t *p) {
  if(emptyProcQ(head)) {
    return NULL;	//lista vuota
  } else {
		// la lista ha almeno un elemento
		struct list_head* scan;
		list_for_each(scan, head) {
			if(container_of(scan, pcb_t, p_next) == p){
		    list_del(scan);
		    return p;
			}
		}
  	return NULL;
  }
}

/* TREE VIEW FUNCTIONS */

/* TRUE se il PCB p non ha figli */
int emptyChild(pcb_t *p) {
	return(list_empty(&p->p_child));
}

/* Inserisce il PCB p come ultimo figlio del PCB prnt */
void insertChild(pcb_t *prnt, pcb_t *p) {
	p->p_parent = prnt;
	list_add_tail(&(p->p_sib), &(prnt->p_child));				//aggiungo il child in fondo ai fratelli
}																	//concatenando alla lista dei figli di prnt il campo p_sib di p

/* Rimuove il primo figlio del PCB prnt e lo restituisce
 * senza padre. Restituisce NULL se prnt non ha figli */
pcb_t *removeChild(pcb_t *p) {
	if(emptyChild(p))												//p non ha figli
		return NULL;
	struct list_head* children = &(p->p_child);						//lista dei figli di p
	pcb_t* first = container_of(children->next, pcb_t, p_sib);		//pcb primo figlio
	list_del(&first->p_sib);										//eliminazione del primo figlio (tolgo il collegamento ai fratelli)
	first->p_parent = NULL;											//aggiorno il suo parent perchè non è più figlio
	return first;
}

/* Rimuove  il PCB p dalla lista dei figli di suo padre
 * Se il padre è NULL restituisce NULL */
pcb_t *outChild(pcb_t *p) {
	if(p->p_parent == NULL)
		return NULL;		//p è senza padre
	list_del(&p->p_sib);	//elimino il puntatore (&p->p_sib) del pcb p dalla lista dei suoi fratelli (quindi elimino il pcb dalla lista dei figli del padre)
	p->p_parent = NULL;		//aggiorno il suo parent perchè non è più figlio
	return p;
}

/* USER DEFINED FUNCTIONS */

/* Inizializzazione struttura del PCB */
void inizializzapcb(struct pcb_t* p) {
	p->p_next.next = NULL;
	p->p_next.prev = NULL;
	p->p_sib.next = NULL;
	p->p_sib.prev = NULL;
	INIT_LIST_HEAD(&p->p_child);
	p->p_parent = NULL;
	initPcbState(&p->p_s);
	p->priority = 0;
	p->p_semkey = NULL;
}

/* Inizializzazione stato del PCB */
void initPcbState(state_t* state) {
	state->entry_hi = 0;
	state->cause = 0;
	state->status = 0;
	state->pc_epc = 0;
	for(int i=0; i<STATE_GPR_LEN; i++)
		state->gpr[i] = 0;
	state->hi = 0;
	state->lo = 0;
}
