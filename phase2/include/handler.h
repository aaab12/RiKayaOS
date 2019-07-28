#ifndef _HANDLER_H
#define _HANDLER_H

#define SYSCALL_TYPE  0
#define TLB_TYPE      1
#define TRAP_TYPE     2

void sysbk_handler();

void pgmtrap_handler();

void tlb_handler();

void int_handler();

#endif
