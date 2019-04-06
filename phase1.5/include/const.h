#ifndef _CONST_H
#define _CONST_H

#include <umps/cp0.h>

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

/* Physical Address of the Interval Timer */
#define BUS_INTERVALTIMER 0x10000020

/* STATUS */
#define EXCEPTION_STATUS 0x18000000 /* EXCEPTION STATUS: Interrupt mascherati, VM off, Kernel mode e PLT on */
#define PROCESS_STATUS_1 0x18007F04 /* PROCESS STATUS: Interrupt abilitati, VM off, Kernel mode e PLT on */
#define PROCESS_STATUS_2 0x10007F04 /* PROCESS STATUS: Interrupt abilitati, VM off, Kernel mode e PLT off */

/* SCHEDULER */
#define SCHED_TIME_SLICE 3000 /* 3000 microsecondi = 3 millisecondi */
#define TIME_SCALE 0x10000024 /* locazione del numero di tick del clock per microsecondo */
#define TIME_SLICE SCHED_TIME_SLICE*(*(memaddr *)TIME_SCALE) /* BUS_TIMESCALE*3000 clock_ticks = 3ms */

/* Macro che restituisce true se la interrupt è del tipo specificato */
#define CAUSE_INT_GET(cause, int_number) ((cause) & CAUSE_IP(int_number))

/* Numero delle linee di interrupt totali */
#define INT_LINES 8

/* INTERRUPTS */
#define INT_PROCESSOR 0
#define INT_PLT	1
#define INT_TIMER 2
#define INT_DISK 3
#define INT_TAPE 4
#define INT_NETWORK 5
#define INT_PRINTER 6
#define INT_TERMINAL 7

/* SYSCALL */
#define SYS1 		          1
#define	SYS2 		          2
#define SYS3              3
#define TERMINATEPROCESS  (SYS3) /* Sulle slide la syscall 3 viene chiamata in modo diverso rispetto al file di test */
#define SYS4		          4
#define SYS5		          5
#define SYS6		          6
#define SYS7		          7
#define SYS8			        8
#define SYS9		          9
#define SYS10		          10
#define SYS11		          11

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
#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)

#define FRAMESIZE 4096 /* 0x1000 bytes = 4K */
#define WORDSIZE 4

#define STEPS 6
#define GANTT_SIZE 20

/* Utility definitions for the Cause CP0 register */
#define CAUSE_EXCCODE_GET(cause) (((cause) & 0x0000007c) >> 2)
#define CAUSE_EXCCODE_SET(cause, exc_code) (((cause) & 0xffffff83) | ((exc_code) << 2))
#define CAUSE_CE_GET(cause) (cause & 0x30000000)

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
