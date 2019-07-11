#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "main.h"
#include "pcb.h"

void get_cpu_time(unsigned int *user, unsigned int *kernel, unsigned int *wallclock);

int create_process(state_t *statep, int priority, void ** cpid);

void terminate_process(void ** pid);

//void verhogen(int *semaddr);

//void passeren(int *semaddr);

//void wait_clock();

//int do_io(unsigned int command, unsigned int *register);

//void set_tutor();

//int spec_passup(int type, state_t *old, state_t *new);

#endif
