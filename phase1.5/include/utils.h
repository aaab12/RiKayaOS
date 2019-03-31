#ifndef UTILS_H
#define UTILS_H

#include "const.h"
#include "listx.h"
#include "pcb.h"

void save_state(state_t* source, state_t* destination);

void aging(struct list_head* head);

#endif
