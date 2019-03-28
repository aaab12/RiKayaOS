#ifndef _CONST_H
#define _CONST_H

/****************************************************************************
 *
 * This header file contains the global constant & macro definitions.
 *
 ****************************************************************************/

/* Maxi number of overall (eg, system, daemons, user) concurrent processes */
#define MAXPROC 20

#define UPROCMAX 3  /* number of usermode processes (not including master proc and system daemons) */

/* Indirizzi delle new e old area in cui gli stati del processore
 * sono memorizzati quando vengono sollevate delle eccezioni
 */
#define SYSBK_NEWAREA 0x200003d4
#define SYSBK_OLDAREA 0x20000348
#define PGMTRAP_NEWAREA 0x200002bc
#define PGMTRAP_OLDAREA 0x20000230
#define TLB_NEWAREA 0x200001a4
#define TLB_OLDAREA 0x20000118
#define INT_NEWAREA 0x2000008c
#define INT_OLDAREA 0x20000000

/* STATUS */
#define EXCEPTION_STATUS 0x18000000 /* EXCEPTION STATUS: Interrupt mascherati, VM off, Kernel mode e PLT on */
#define PROCESS_STATUS 0x1800FF01 /* PROCESS STATUS: Interrupt abilitati, VM off, Kernel mode e PLT on */

/* SCHEDULER */
#define SCHED_TIME_SLICE 3000 /* 3000 microsecondi = 3 millisecondi */
#define BUS_TIMESCALE 0x10000024 /* numero di tick del clock per microsecondo */
#define TIME_SLICE SCHED_TIME_SLICE*(*(memaddr *)BUS_TIMESCALE) /* BUS_TIMESCALE*3000 clock_ticks = 3ms */

/* SYSCALL */
/* Sulle slide la syscall 3 viene chiamata in modo diverso rispetto al file di test */
#define SYS3              3
#define TERMINATEPROCESS  (SYS3)

#define TRANSMITTED 5
#define TRANSTATUS 2
#define ACK 1
#define PRINTCHR 2
#define CHAROFFSET 8
#define STATUSMASK 0xFF
#define TERM0ADDR 0x10000250
#define DEVREGSIZE 16
#define READY 1
#define DEVREGLEN 4
#define TRANCOMMAND 3
#define BUSY 3

#define TOD_LO *((unsigned int *)0x1000001C)
#define TIME_SCALE *((unsigned int *)0x10000024)
#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)

#define FRAMESIZE 4096 /* 0x1000 bytes = 4K */
#define WORDSIZE 4

#define STEPS 6
#define GANTT_SIZE 20

#define	HIDDEN static
#define	TRUE 	1
#define	FALSE	0
#define ON 	1
#define OFF 	0
#define EOS '\0'

#define DEV_PER_INT 8 /* Maximum number of devices per interrupt line */

#define NULL ((void *)0)

#define CR 0x0a   /* carriage return as returned by the terminal */

#endif
