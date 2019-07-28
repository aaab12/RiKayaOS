#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define STATUS_ALL_INT_ENABLE(x)    (x | (0xFF << 8))

void scheduler();

#endif
