#include "const.h"
#include "listx.h"
#include <umps/libumps.h>

#include "pcb.h"
#include "asl.h"

// semd_table: array dei semafori free
struct semd_t semd_table[MAXPROC];
// semdFree_h: sentinella semdFree
struct list_head semdFree_h =  LIST_HEAD_INIT(semdFree_h);
// semdFree: puntatore alla lista dei semafori free
struct list_head* semdFree = &semdFree_h;

// semd_h: sentinella ASL
struct list_head semd_h =  LIST_HEAD_INIT(semd_h);
// ASL lista semafori attivi
struct list_head* ASL = &semd_h;

/* ASL handling functions */

semd_t* getSemd(int *key) {
  // cerco semaforo con chiave key, il controllo da lo stesso risultato sia per semaforo non trovato che per lista vuota
  list_head* scanasl = ASL->next;
  semd_t *scansem;
  while(scanasl != ASL) {
    scansem = container_of(scanasl, semd_t, s_next);
    if(scansem->s_key == key) {
      return scansem;
    } else {
      scanasl = scanasl->next;
    }
  }
  return NULL;
}

int insertBlocked(int *key,pcb_t* p) {
  // il PCB viene inserito in fondo alla coda dei processi bloccati sul semaforo con chiave key
  semd_t *ptr = getSemd(key);
  if(ptr == NULL) {
    // il semaforo con key non è presente nella ASL
    if(!list_empty(semdFree)) {
      // ultimo semaforo fra i liberi
      list_head *s = semdFree->prev;
      semd_t *se = container_of(s, semd_t, s_next);
      // tolgo il semaforo dalla semdFree
      list_del(s);
      // aggiungo il semaforo alla ASL
      list_add_tail(&se->s_next, ASL);
      se->s_key = key;
      se->s_procQ.next = &p->p_next;
      p->p_next.prev = &se->s_procQ;
      p->p_semkey = key;
      return 0;
    } else {
      // semaforo con key non presente nella ASL e non ci sono semafori liberi da allocare
      return 1;
    }
  } else {
    // ho trovato il semaforo con quella key nella ASL
    list_head *scan = &ptr->s_procQ;
    while(scan->next != NULL) {
      scan = scan->next;
    }
    // sono sull'ultimo PCB in coda al semaforo con key
    scan->next = &p->p_next;
    p->p_next.prev = scan;
    p->p_semkey = key;
    return 0;
  }
}

void initASL() {
  int i;
  for(i = 0; i < MAXPROC; i++){
    list_add_tail(&semd_table[i].s_next, semdFree);
  }
}

pcb_t* removeBlocked(int *key){
  semd_t *ptr = getSemd(key);
  if(ptr == NULL) {
    return NULL;
  } else {
    // ho trovato il semaforo con la key (ptr)
    list_head *scanq = &ptr->s_procQ;
    pcb_t *toret = container_of(scanq->next, pcb_t, p_next);
    list_del_notcirc(&toret->p_next);
    inizializzapcb(toret);
    // controllo se il semaforo è rimasto senza processi bloccati in coda, in questo caso lo metto nella lista dei semafori liberi
    if(scanq->next==NULL) {
      // il semaforo è rimasto vuoto
      list_del(&ptr->s_next);
      list_add_tail(&ptr->s_next, semdFree);
    }
    return toret;
  }
}

pcb_t* outBlocked(pcb_t *p) {
  int *key = p->p_semkey;
  semd_t *ptr = getSemd(key);
  if(ptr == NULL){
    // il semaforo su cui è bloccato p non è nella ASL
    return NULL;
  } else {
    // ho trovato il semaforo a cui appartiene il pcb p (semd_t *ptr) che essendo allocato ha almeno un processo
    // bloccato su di esso, sul quale mi sposto (scan)
    pcb_t *scan = container_of(ptr->s_procQ.next, pcb_t, p_next);
    while(scan != p || scan->p_next.next != NULL) {
      scan = container_of(scan->p_next.next, pcb_t, p_next);
    }
    if(scan != p) {
      // il pcb p non era nella coda dei processi bloccati sul semaforo a cui appartiene: ERRORE
      return NULL;
    } else {
      // sono sul pcb da ritornare
      list_del_notcirc(&scan->p_next);
      // controllo se il semaforo è rimasto senza processi bloccati in coda e nel caso lo dealloco
      if(ptr->s_procQ.next == NULL) {
        list_del(&ptr->s_next);
        list_add_tail(&ptr->s_next, semdFree);
      }
      return p;
    }
  }
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
    // p è stato rimosso
    del_tree(p);
  }
}

//USER defined functions
/*
elimina un elemento di una lista bidirezionale non circolare (non deve essere il primo elemento della lista quello da eliminare)
  p: puntatore alla campo list_head della struttura che voglio eliminare (PCB)
      non deve essere il primo elemento

  return: ritorna
*/
list_head* list_del_notcirc(list_head *p) {
  if(p->next == NULL){
    p->prev->next = NULL;
    p->prev = NULL;
    return p;
  } else {
    p->prev->next = p->next;
    p->next->prev = p->prev;
    p->next = NULL;
    p->prev = NULL;
    return p;
  }
}

/*
  elimina dalle relative code di processi dei relativi semafori tutti i processi (PCB) che hanno come avo p (input)
*/
void recursive(pcb_t *root, pcb_t *realroot) {
  list_head *scany = &root->p_child;
  list_head *scanx = &root->p_sib;
  if(scany->next != NULL){
    recursive(container_of(scany->next, pcb_t, p_child), realroot);
  }
  if(scanx->next != NULL && root != realroot){
    recursive(container_of(scanx->next, pcb_t, p_sib), realroot);
  }
  outBlocked(container_of(scanx, pcb_t, p_sib));
}

/*
  funzione ausiliaria alla funzione "recursive": serve per dover dare un solo parametro in input (e non dando lo stesso due volte) rendendo piu intuitiva la chiamata
  e in questo modo tenendone uno dei due fuori dalla ricorsione
*/
void del_tree(pcb_t* p) {
  recursive(p, p);
}

//funzione inizializzazione struttura semd_t
void inizializzasem(struct semd_t* p) {
  p->s_next.next = NULL;
  p->s_next.prev = NULL;
  p->s_key = NULL;
  p->s_procQ.next = NULL;
  p->s_procQ.prev = NULL;
}
