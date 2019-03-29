#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <umps/libumps.h>
#include "const.h"
#include "listx.h"
#include "pcb.h"
#include "interrupt.h"

state_t* int_old_area;
int int_cause;

void int_timer();



#endif
