#ifndef _CONST_H
#define _CONST_H

/****************************************************************************
 *
 * This header file contains the global constant & macro definitions.
 *
 ****************************************************************************/

/* STATUS */
#define EXCEPTION_STATUS 0x18000000 /* EXCEPTION STATUS: Interrupt mascherati, VM off, Kernel mode e PLT on */

#define PROCESS_STATUS 0x18007F04 /* PROCESS STATUS 1: Interrupt abilitati, VM off, Kernel mode e PLT on */
// 0001 1 000 0 0 000000 01111111 00 000100
// CP_U|T|V_M|x|B|xxxxxx|INT_MASK|xx|KU__IE

/* SCHEDULER */
#define TIME_SLICE SCHED_TIME_SLICE*(*(memaddr *)TIME_SCALE) /* BUS_TIMESCALE*4000 clock_ticks = 4ms */

#endif
