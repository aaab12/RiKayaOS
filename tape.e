#ifndef TAPE_E
#define TAPE_E

#include "umps/types.h"

extern int tape_read(char *buffer, dtpreg_t *dtp);

extern void tape_skip(dtpreg_t *dtp);

extern uint8_t tape_end(dtpreg_t *dtp);

#endif
