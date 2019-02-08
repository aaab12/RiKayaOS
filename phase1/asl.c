


#include "const.h"
#include "listx.h"
#include <umps/libumps.h>

#include "pcb.h"
#include "asl.h"


//semd_table: array dei semafori free
struct semd_t semd_table[MAXPROC];
//semdFree_h: sentinella semdFree
struct list_head semdFree_h =  LIST_HEAD_INIT(semdFree_h);
//semdFree: puntatore alla lista dei semafori free
struct list_head* semdFree = &semdFree_h;

//semd_h: sentinella ASL
struct list_head semd_h =  LIST_HEAD_INIT(semd_h);
//ASL lista semafori attivi
struct list_head* ASL = &semd_h;







/* ASL handling functions */
/*
semd_t* getSemd(int *key){

  if(list_empty(ASL)){
    return NULL;
  } else {
    list_head* scanasl = ASL->next;
    //QUI POSSO METTERE ANCHE SOLO IL SECONDO CONTROLLO, LISTA CIRCOLARE?
    while(scanasl->next != NULL || scanasl->next != &(*scanasl)){
      semd_t* tmp = container_of(scanasl, semd_t, s_next);
      int** p = &tmp->s_key;
      if(**p == *key){
        return tmp;
      } else {
        scanasl = scanasl->next;
      }

    }
    semd_t* tmp = container_of(scanasl, semd_t, s_next);
    int** p = &tmp->s_key;
    if(**p == *key){
      return tmp;
    } else {
      return NULL;
    }

  }

}
*/



semd_t* getSemd(int *key){
  //cerco semaforo con chiave key, il controllo da lo stesso risultato sia per semaforo non trovato che per lista vuota
  list_head* scanasl = ASL->next;
  semd_t *scansem;
  while(scanasl!=ASL)
  {
    scansem = container_of(scanasl, semd_t, s_next);
    if(scansem->s_key==key)
    {
      return scansem;
    } else
    {
      scanasl = scanasl->next;
    }
  }
  return NULL;
}


int insertBlocked(int *key,pcb_t* p){
  semd_t *ptr = getSemd(key);
  if(ptr == NULL){
    if(!list_empty(semdFree)){
      //ultimo semaforo fra i liberi
      list_head *s = semdFree->prev;
      semd_t *se = container_of(s, semd_t, s_next);
      list_del(s);
      //aggiungo il semaforo alla ASL
      list_add_tail(&se->s_next, ASL);
      se->s_key=key;
      se->s_procQ.next=&p->p_next;
      p->p_next.prev = &se->s_procQ;
      p->p_semkey=key;
      return 0;
    } else
    {
      //semaforo con key non presente nella asl, lista semafori liberi vuota
      return 1;
    }

  } else
  {
    //ho trovato il semaforo con quella key nella ASL
    list_head *scan = &ptr->s_procQ;
    while(scan->next != NULL){
      scan = scan->next;
    }
    scan->next = &p->p_next;
    p->p_next.prev = scan;
    p->p_semkey = key;
    return 0;
  }
}


void initASL(){

  int i;

  for(i = 0; i < MAXPROC; i++){
    struct list_head* ptr = &semd_table[i].s_next;
    list_add_tail(ptr, semdFree);
  }

}


/*
int insertBlocked(int *key,pcb_t* p){
  semd_t *scansem = container_of(ASL->next, semd_t, s_next);
  list_head *scansnext= &scansem->s_next;
  while(scansem->s_key != key||list_is_last(scansnext, ASL)){
    //vedo se ho un semaforo con la key che cerco nella ASL
    scansnext = scansnext->next;
    scansem = container_of(scansnext, semd_t, s_next);
  }
  if(scansem->s_key == key){
    //ho trovato il semaforo con la key specificata (scansem), non so se sia l'ultimo o meno
    //list_add_tail(&p->p_next, &scansem->s_procQ);
    insertProcQ(&scansem->s_procQ, p);
    return FALSE;

  } else {
    //il semaforo con la key specificata non e nella ASL

    //cerco di allocare un nuovo semaforo da quelli liberi
    if(list_empty(semdFree)){
    //non ci sono semafori liberi
      return TRUE;
    } else {
      //alloco nuovo semaforo
      list_head *lastfree = semdFree->prev;

      list_del(lastfree);
      list_add_tail(lastfree, ASL);
      //inizializzo campo s_procQ del nuovo semaforo
      semd_t *lastsem = container_of(lastfree, semd_t, s_next);
      lastsem->s_procQ.next = &lastsem->s_procQ;
      //controllo se questo nuovo semaforo e l'unico semaforo di asl
      if(lastsem->s_next.next == ASL && lastsem->s_next.prev == ASL){
        *(lastsem->s_key) = 1;
      } else {
        semd_t *lastprevsem = container_of(lastsem->s_next.prev, semd_t, s_next);
        *(lastsem->s_key) = *(lastprevsem->s_key)+1;
      }
      //aggancio pcb a nuovo semaforo
      list_add_tail(&p->p_next, &lastsem->s_procQ);
      return FALSE;
    }



  }


}
*/





pcb_t* removeBlocked(int *key){
  semd_t *ptr = getSemd(key);
  if(ptr == NULL)
  {
    return NULL;
  } else
  {
    //ho trovato il semaforo con la key (ptr)
    list_head *scanq = &ptr->s_procQ;
    pcb_t *toret = container_of(scanq->next, pcb_t, p_next);
    list_del_notcirc(&toret->p_next);
    inizializzapcb(toret);
    if(scanq->next==NULL){
      //il semaforo e rimasto vuoto
      list_del(&ptr->s_next);
      list_add_tail(&ptr->s_next, semdFree);

    }
    return toret;
  }

}






pcb_t* outBlocked(pcb_t *p){
  int *key = p->p_semkey;
  semd_t *ptr = getSemd(key);
  if(ptr == NULL){
    //DEVO GESTIRE QUESTO CASO??:NON E SPECIFICATO NELLE SLIDE
    return NULL;
  } else
  { //ho trovato il semaforo a cui appartiene il pcb p (semd_t *ptr)
    pcb_t *scan = container_of(ptr->s_procQ.next, pcb_t, p_next);
    while(scan != p || scan->p_next.next!=NULL)
    {
      scan = container_of(scan->p_next.next, pcb_t, p_next);
    }
    if(scan != p){
      return NULL; //ERRORE
    } else {
      //sono sul pcb da ritornare
      list_del_notcirc(&scan->p_next);
      if(ptr->s_procQ.next == NULL){
        list_del(&ptr->s_next);
        list_add_tail(&ptr->s_next, semdFree);
      }
      return p;
    }


  }

}







pcb_t* headBlocked(int *key){
  semd_t *ptr = getSemd(key);
  if(ptr == NULL){
    //il semaforo con quella chiave non e in ASL
    return NULL;
  } else
  {
    if(ptr->s_procQ.next==NULL){
      return NULL;
    } else
    {
      return container_of(ptr->s_procQ.next, pcb_t, p_next);
    }

  }

}




void outChildBlocked(pcb_t *p){
  pcb_t *rad = outBlocked(p);
  if(rad != NULL){

    //p e stato rimosso
    del_tree(p);



  }


}








list_head* list_del_notcirc(list_head *p){
  if(p->next==NULL){
    p->prev->next = NULL;
    p->prev = NULL;
    return p;
  } else
  {
    p->prev->next = p->next;
    p->next->prev = p->prev;
    p->next = NULL;
    p->prev = NULL;
    return p;
  }
}



void recursive(pcb_t *root, pcb_t *realroot){
  list_head *scany = &root->p_child;
  list_head *scanx = &root->p_sib;
  if(scany->next!=NULL){
    recursive(container_of(scany->next, pcb_t, p_child), realroot);
  }
  if(scanx->next!=NULL&&root!=realroot){
    recursive(container_of(scanx->next, pcb_t, p_sib), realroot);
  }
  pcb_t *tmp = outBlocked(container_of(scanx, pcb_t, p_sib));


}


void del_tree(pcb_t* p){
  recursive(p, p);
}


void inizializzasem(struct semd_t* p){
  p->s_next.next = NULL;
  p->s_next.prev = NULL;
  p->s_key = NULL;
  p->s_procQ.next = NULL;
  p->s_procQ.prev = NULL;


}
