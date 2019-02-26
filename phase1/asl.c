#include "const.h"
#include "listx.h"
#include <umps/libumps.h>

#include "pcb.h"
#include "asl.h"

// semd_table: array dei semafori free
HIDDEN struct semd_t semd_table[MAXPROC];
// semdFree_h: sentinella semdFree
HIDDEN struct list_head semdFree_h;
// semdFree: puntatore alla lista dei semafori free
HIDDEN struct list_head* semdFree = &semdFree_h;

// semd_h: sentinella ASL
HIDDEN struct list_head semd_h;
// ASL lista semafori attivi
HIDDEN struct list_head* ASL = &semd_h;

/* ASL HANDLING FUNCTIONS */

/* Restituisce il semaforo di chiave key presente in ASL
 * NULL se non presente */
semd_t* getSemd(int *key) {
  struct list_head* scanasl;
  semd_t *scansem;
  list_for_each(scanasl, ASL) {							//scansione lista
	  scansem = container_of(scanasl, semd_t, s_next);
	  if(scansem->s_key == key)
		return scansem;
	}
	return NULL;
  }

/* Inizializzazione struttura ASL contenente tutti gli elementi della semdTable */
void initASL() {
	INIT_LIST_HEAD(&semdFree_h);		//sentinella della lista dei semafori liberi
	INIT_LIST_HEAD(&semd_h);			//sentinella della lista dei semafori attivi
	int i;
	for(i = 0; i < MAXPROC; i++){
		list_add_tail(&semd_table[i].s_next, semdFree);
	}
}

/* Inserisce  il PCB p nella ASL nella coda dei processi bloccati del semaforo con Id key
 * Per l'inserimento si usa la funzione insertProcQ di Queue Handling Process */
int insertBlocked(int *key,pcb_t* p) {
  // il PCB viene inserito nella coda dei processi bloccati sul semaforo con chiave key in base alla priorità
  semd_t *ptr = getSemd(key);
  if(ptr == NULL) {                              	 // il semaforo con key non è presente nella ASL
    if(list_empty(semdFree))	                 	 // semaforo con key non presente nella ASL e non ci sono semafori liberi da allocare
		return 1;
    list_head *s = semdFree->prev;                	// ultimo semaforo fra i liberi
    semd_t *se = container_of(s, semd_t, s_next);
    list_del(s);                                 	 // tolgo il semaforo dalla semdFree
    list_add_tail(&se->s_next, ASL);             	 // aggiungo il semaforo in coda alla ASL
    se->s_key = key;							 	 // inizializzo key e lista dei processi bloccati del semaforo aggiunto
    mkEmptyProcQ(&se->s_procQ);
    p->p_semkey = key;								 // aggiorno la semkey del PCB p
    insertProcQ(&se->s_procQ, p);               	 // aggiungo il pcb alla coda dei processi bloccati dal semaforo
    return 0;
  }
  // ho trovato il semaforo con quella key nella ASL
  p->p_semkey = key;								// aggiorno la semkey del PCB p
  insertProcQ(&ptr->s_procQ, p);					// aggiungo il pcb alla coda dei processi bloccati dal semaforo
  return 0;
}

/* Ritorna il primo PCB della coda dei processi bloccati associata al semaforo di chiave
 * key presente nella ASL. Se key non presente restituisce NULL altrimenti restituisce
 * il PCB rimosso. Se la coda del semaforo si svuota il semaforo è aggiunto alla semdFree */
pcb_t* removeBlocked(int *key){
  semd_t *ptr = getSemd(key);
  if(ptr == NULL) {
    return NULL;
    }
    // ho trovato il semaforo con la key (ptr)
    pcb_t *p = removeProcQ(&ptr->s_procQ);				// PCB rimosso
    // controllo se il semaforo è rimasto senza processi bloccati in coda, in questo caso lo metto nella lista dei semafori liberi
    if(emptyProcQ(&ptr->s_procQ)) {
      // il semaforo è rimasto vuoto: lo rimuovo dalla ASL e lo inserisco in coda alla lista dei semdFree
      list_del(&ptr->s_next);
      list_add_tail(&ptr->s_next, semdFree);
    }
    return p;											// ritorno il PCB rimosso
}

/* Rimuove il PCB p dalla coda del semaforo su cui è bloccato (key = p_semdkey)
 * Se il PCB non compare nella coda restituisce NULL (ERRORE) altrimenti restituisce p */
pcb_t* outBlocked(pcb_t *p) {
    int *key = p->p_semkey;
    semd_t *ptr = getSemd(key);
    if(ptr == NULL)                  		 	// il semaforo su cui è bloccato p non è nella ASL
        return NULL;
    // ho trovato il semaforo a cui appartiene il pcb p (semd_t *ptr)
    pcb_t *pout = outProcQ(&ptr->s_procQ, p);	//rimuovo p dalla lista del semaforo trovato
    if(pout == NULL)
        return NULL;                  			// il pcb p non era nella coda dei processi bloccati sul semaforo a cui appartiene: ERRORE
    if(emptyProcQ(&ptr->s_procQ)) {
      // il semaforo è rimasto vuoto: lo rimuovo dalla ASL e lo inserisco in coda alla lista dei semdFree
      list_del(&ptr->s_next);
      list_add_tail(&ptr->s_next, semdFree);
    }
    return pout;
}

/* Restituisce senza rimuovere il puntatore al PCB che si trova in testa alla coda dei
 * processi associata al semaforo di chiave key. Ritorna NULL se il semaforo non è
 * nella ASL oppure se compare ma ha la lista dei processi vuota */
pcb_t* headBlocked(int *key) {
  semd_t *ptr = getSemd(key);
  if(ptr == NULL || list_empty(&ptr->s_procQ)) {
    // il semaforo con quella chiave non è in ASL o la sua lista è vuota
    return NULL;
  } else {
    // ritorno il primo PCB bloccato in coda al semaforo
    return container_of(ptr->s_procQ.next, pcb_t, p_next);
  }
}

/* Rimuove il PCB p dalla coda del semaforo su cui è bloccato (key = p_semdkey)
 * Inoltre elimina con una previsita dell'albero radicato in p tutti i processi
 * dalle eventuali code dei semafori in cui sono bloccati */
void outChildBlocked(pcb_t *p) {
  pcb_t *rad = outBlocked(p);
  if(rad != NULL){
    // p è stato rimosso e itero ricorsivamente su ogni suo discendente
     delTree(p);
  }
}

/* USER DEFINED FUNCTIONS */

/* Funzione chiamata da outChildBlocked*/
void delTree(pcb_t *p) {
    if(emptyChild(p))
        return;                                             //se p non ha figli ho finito altrimenti
    list_head* scan;                                        //itero sulla lista dei figli di p
    list_for_each(scan, &p->p_child) {
        outBlocked(container_of(scan, pcb_t, p_sib));       //rimuovo il pcb di ogni figlio dalla coda del semaforo in cui è bloccato nelle liste di tutti i fratelli
        delTree(container_of(scan, pcb_t, p_sib));          //itero ricorsivamente sulla lista dei figli di ogni figlio di p
    }
}
