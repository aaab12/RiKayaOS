#include "const.h"
#include "listx.h"
#include <umps/libumps.h>

#include "pcb.h"
#include "asl.h"

// semd_table: array dei semafori free
struct semd_t semd_table[MAXPROC];
// semdFree_h: sentinella semdFree
struct list_head semdFree_h;
// semdFree: puntatore alla lista dei semafori free
struct list_head* semdFree = &semdFree_h;

// semd_h: sentinella ASL
struct list_head semd_h;
// ASL lista semafori attivi
struct list_head* ASL = &semd_h;

/* ASL handling functions */

semd_t* getSemd(int *key) {
  // cerco semaforo con chiave key, il controllo da lo stesso risultato sia per semaforo non trovato che per lista vuota
  struct list_head* scanasl;
  semd_t *scansem;
  list_for_each(scanasl, ASL) {
	  scansem = container_of(scanasl, semd_t, s_next);
	  if(scansem->s_key == key)
		return scansem;
	}
	return NULL;	
  }

void initASL() {
	INIT_LIST_HEAD(&semdFree_h);
	INIT_LIST_HEAD(&semd_h);
	int i;
	for(i = 0; i < MAXPROC; i++){
		list_add_tail(&semd_table[i].s_next, semdFree);
	}
}

int insertBlocked(int *key,pcb_t* p) {
  // il PCB viene inserito nella coda dei processi bloccati sul semaforo con chiave key in base alla priorità
  semd_t *ptr = getSemd(key);
  if(ptr == NULL) {                                   // il semaforo con key non è presente nella ASL
    if(list_empty(semdFree))	                      // semaforo con key non presente nella ASL e non ci sono semafori liberi da allocare
		return 1;
      list_head *s = semdFree->prev;                  // ultimo semaforo fra i liberi
      semd_t *se = container_of(s, semd_t, s_next);
      list_del(s);                                    // tolgo il semaforo dalla semdFree
      list_add_tail(&se->s_next, ASL);                // aggiungo il semaforo alla ASL
      se->s_key = key;
      mkEmptyProcQ(&se->s_procQ);
      p->p_semkey = key;
      insertProcQ(&se->s_procQ, p);                   // aggiungo il pcb alla coda dei processi bloccati dal semaforo
      return 0;
  }
  // ho trovato il semaforo con quella key nella ASL
  p->p_semkey = key;
  insertProcQ(&ptr->s_procQ, p);
  return 0;
}

pcb_t* removeBlocked(int *key){
  semd_t *ptr = getSemd(key);
  if(ptr == NULL) {
    return NULL;
    }
    // ho trovato il semaforo con la key (ptr)
    pcb_t *p = removeProcQ(&ptr->s_procQ);
    // controllo se il semaforo è rimasto senza processi bloccati in coda, in questo caso lo metto nella lista dei semafori liberi
    if(emptyProcQ(&ptr->s_procQ)) {
      // il semaforo è rimasto vuoto
      list_del(&ptr->s_next);
      list_add_tail(&ptr->s_next, semdFree);
    }
    return p;
}

pcb_t* outBlocked(pcb_t *p) {
    int *key = p->p_semkey;
    semd_t *ptr = getSemd(key);
    if(ptr == NULL)                   // il semaforo su cui è bloccato p non è nella ASL
        return NULL;
    // ho trovato il semaforo a cui appartiene il pcb p (semd_t *ptr) che essendo allocato ha almeno un processo
    // bloccato su di esso, sul quale mi sposto (scan)
    pcb_t *pout = outProcQ(&ptr->s_procQ, p);
    if(pout == NULL)
        return NULL;                  // il pcb p non era nella coda dei processi bloccati sul semaforo a cui appartiene: ERRORE
    if(emptyProcQ(&ptr->s_procQ)) {
      // il semaforo è rimasto vuoto
      list_del(&ptr->s_next);
      list_add_tail(&ptr->s_next, semdFree);
    }
    return pout;
}

pcb_t* headBlocked(int *key) {
  semd_t *ptr = getSemd(key);
  if(ptr == NULL) {
    // il semaforo con quella chiave non è in ASL
    return NULL;
  } else {
    // ritorno il primo PCB bloccato in coda al semaforo. Non può essere che non ce ne sia almeno uno o il semaforo non sarebbe nella ASL
    return container_of(ptr->s_procQ.next, pcb_t, p_next);
  }
}

void outChildBlocked(pcb_t *p) {
  pcb_t *rad = outBlocked(p); 
  if(rad != NULL){
    // p è stato rimosso e itero ricorsivamente su ogni suo discendente 
     delTree(p);
  }
}

void delTree(pcb_t *p) {
    if(emptyChild(p))
        return;                                             //se p non ha figli ho finito
    list_head* scan;                                        //itero sulla lista dei figli di p
    list_for_each(scan, &p->p_child) {
        outBlocked(container_of(scan, pcb_t, p_sib));       //rimuovo il pcb di ogni figlio dalla coda del semaforo in cui è bloccato nelle liste di tutti i fratelli
        delTree(container_of(scan, pcb_t, p_sib));          //itero ricorsivamente sulla lista dei figli di ogni figlio di p
    }          
}


