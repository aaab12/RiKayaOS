#ifndef _HANDLER_H
#define _HANDLER_H

#include "const_rikaya.h"
#include "interrupt.h"
#include <umps/libumps.h>
#include "scheduler.h"
#include "syscall.h"
#include "types_rikaya.h"
#include "utils.h"

void sysbk_handler();

void pgmtrap_handler();

void tlb_handler();

void int_handler();

#endif
