#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <umps/types.h>
#include "types_rikaya.h"

void get_cpu_time(unsigned int *user, unsigned int *kernel, unsigned int *wallclock);

int create_process(state_t *statep, int priority, void ** cpid);

int terminate_process(void ** pid);

void verhogen(int *semaddr);

pcb_t* verhogen_2(int *semaddr);

void passeren(int *semaddr);

void wait_clock();

U32 do_io(unsigned int command, unsigned int *reg, unsigned int rw);

void set_tutor();

int spec_passup(int type, state_t *old, state_t *new);

void get_pid_ppid(void **pid, void **ppid);

#endif
