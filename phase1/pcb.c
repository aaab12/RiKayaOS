

#include "const.h"
#include "listx.h"
#include <umps/libumps.h>

#include "pcb.h"
#include "asl.h"
//#define MAX_PROC 20



//pcbFree_table: array dei pcb free
struct pcb_t pcbFree_table[MAXPROC];
//pcbfree_h: sentinella pcbFree
struct list_head pcbfree_h =  LIST_HEAD_INIT(pcbfree_h);
//pcbFree: puntatore alla lista dei pcb free
struct list_head* pcbFree = &pcbfree_h;





/* PCB handling functions */

/* PCB free list handling functions */

void initPcbs(){
	int i;
  for(i = 0; i < MAXPROC; i++){
    struct list_head* ptr = &pcbFree_table[i].p_next;
    list_add_tail(ptr, pcbFree);
  }
}


void freePcb(pcb_t * p){

	//qui potrei mettere un controllo che il pcb non sia gia nella lista dei pcbFree
	struct list_head* ptr = &p->p_next;
	list_add_tail(ptr, pcbFree);
}



pcb_t* allocPcb(){
	if(list_empty(pcbFree))
	{
		return NULL;
	} else
	{
		struct list_head* ptr = pcbFree->prev;
		//pcb ultimo elemento
    struct pcb_t * ptrp = container_of(ptr, struct pcb_t, p_next);
		__list_del(pcbFree->prev->prev, pcbFree);
    inizializzapcb(ptrp);
		return ptrp;
	}
}


/* PCB queue handling functions */
void mkEmptyProcQ(struct list_head *head){
  INIT_LIST_HEAD(head);
  return;
}

int emptyProcQ(struct list_head *head){
  return list_empty(head);
}


void insertProcQ(struct list_head *head, pcb_t *p){
  struct list_head *toadd = &p->p_next;
  if(list_empty(head)){
    list_add_tail(toadd, head);
    return;
  } else
	{
    struct list_head *scan = head->next;
		struct pcb_t* cmp = container_of(scan, pcb_t, p_next);
    while(scan->next!=head)
		{
      if(cmp->priority < p->priority)
			{
        __list_add(toadd, scan->prev, scan);
        return;
      } else
			{
        scan = scan->next;
				cmp = container_of(scan, pcb_t, p_next);
      }
    }
		//sono sull'ultimo pcb
    if(cmp->priority < p->priority){
      __list_add(toadd, scan->prev, scan);
    } else {
			//in questo caso scan->next punta a head
      __list_add(toadd, scan, scan->next);
    }
	return;
  }
}


pcb_t *headProcQ(struct list_head *head){
  if(list_empty(head)){
    return NULL;
  } else
	{
    struct pcb_t* first = container_of(head->next, pcb_t, p_next);
    return first;
  }
}

pcb_t *removeProcQ(struct list_head *head){
  if(list_empty(head)){
    return NULL;
  } else
	{
    struct list_head *ptr = head->next;
    struct pcb_t* torem = container_of(ptr, pcb_t, p_next);
    __list_del(head, ptr->next);
    return torem;
  }

}


pcb_t *outProcQ(struct list_head *head, pcb_t *p){
  if(list_empty(head)){
    return NULL;
  } else
	{
    struct list_head *scanh = head->next;
		struct pcb_t *scanp;
    while(scanh!=head)
		{
			scanp = container_of(scanh, pcb_t, p_next);
      if(scanp==p){
        __list_del(scanh->prev, scanh->next);
        return scanp;
      } else {
        scanh=scanh->next;
      }
    }
  return NULL;
  }
}



/* Tree view functions */
int emptyChild(pcb_t *this){
  if(this->p_child.next == NULL){
    return 1;
  } else {
    return 0;
  }
}




void insertChild(pcb_t *prnt, pcb_t *p){
  struct list_head* toaddc = &p->p_child;
  struct list_head* toadds = &p->p_sib;
  struct list_head *scanh = &prnt->p_child;
  if(scanh->next == NULL){
		//QUI DOVREI GIA AVER PENSATO AD INIZIALIZZARE P?
		//prnt non ha figli
    scanh->next = toaddc;
    p->p_parent = prnt;
    return;
  } else
	{



		//scanp = primo figlio di prnt
    pcb_t* scanp = container_of(scanh->next, pcb_t, p_child);
    struct list_head *scanhsib = &scanp->p_sib;

    while(scanhsib->next != NULL){
      scanhsib = scanhsib->next;
    }
    scanhsib->next = toadds;
		toadds->prev = scanhsib;
    p->p_parent = prnt;
    return;
  }
}


pcb_t *removeChild(pcb_t *p){

  if(p->p_child.next == NULL)
	{
    return NULL;
  } else
	{
		//scanc: primo figlio
    struct list_head *scanc = p->p_child.next;
    struct pcb_t *scanp = container_of(scanc, pcb_t, p_child);
    struct list_head *scansib = &scanp->p_sib;
    if(scansib->next == NULL){
			//scanc: figlio unico
      p->p_child.next = NULL;
      inizializzapcb(scanp);
      return scanp;
    } else {
			//scanc ha fratelli, nextp: primo fratello
      struct pcb_t* nextp = container_of(scansib->next, pcb_t, p_sib);
      struct list_head* nextc = &nextp->p_child;
      p->p_child.next = nextc;
			nextp->p_sib.prev = NULL;
      inizializzapcb(scanp);
      return scanp;
    }
  }
}



pcb_t *outChild(pcb_t *p){
  if(p->p_parent == NULL)
  {
		//p non ha padre
    return NULL;
  } else
	{
    struct pcb_t *prnt = p->p_parent;

    if(prnt->p_child.next == &p->p_child)
    {
      //p e il primo figlio di prnt
      return removeChild(prnt);
    } else
    {
			//prevson: inizializzato primo figlio del padre di p
      struct pcb_t *prevson = container_of(prnt->p_child.next, pcb_t, p_child);
      while(prevson->p_sib.next != &p->p_sib){
        prevson = container_of(prevson->p_sib.next, pcb_t, p_sib);
      }
			//qui prevson e il figlio di prnt prima di p nella lista dei fratelli
      if(p->p_sib.next==NULL)
      {
        //p ultimo figlio
        prevson->p_sib.next=NULL;
        inizializzapcb(p);
        return p;

      } else
      {
				//p non e ultimo figlio
        prevson->p_sib.next = p->p_sib.next;
				pcb_t *nextson = container_of(p->p_sib.next, pcb_t, p_sib);
				nextson->p_sib.prev = &prevson->p_child;
        inizializzapcb(p);
        return p;

      }

      /*
      struct pcb_t *prevson = container_of(prnt->p_child.next, pcb_t, p_child);
      while(prevson->p_sib.next!=&p->p_sib){
        prevson = container_of(prevson->p_sib.next, pcb_t, p_sib);
      }
      if(p->p_sib.next==NULL){
        prevson->p_sib.next = NULL;
        inizializza(p);
        return p;
      } else{
        prevson->p_sib.next = p->p_sib.next;
        inizializza(p);
        return p;
        */
    }
  }
}




//Qui ho assunto che inizializzare le list_head* e i pcb_t* significa = NULL invece potrebbe essere puntare a se, lo stesso per gli altri campi: 0 = init??
//Questo anche nelle funzioni sopra: se un pcb non ha fratelli il list_head* corrispondente (next) punta a null o a se stesso (list_head)??
void inizializzapcb(struct pcb_t* p){
  p->p_next.next = NULL;
  p->p_next.prev = NULL;
  p->p_parent = NULL;
  p->p_child.next = NULL;
  p->p_child.prev = NULL;
  p->p_sib.next = NULL;
  p->p_sib.prev = NULL;
  //p->p_s = 0;
  p->priority = 0;
  p->p_semkey = NULL;

}
