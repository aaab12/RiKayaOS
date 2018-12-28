#ifndef TAPE_E
#define TAPE_E

#include "umps/types.h"

extern int tape_cmd(dtpreg_t *dtp, int cmd);

extern int tape_rewind(dtpreg_t *dtp);

extern int tape_skip(dtpreg_t *dtp);

extern int tape_back(dtpreg_t *dtp);

extern int tape_read(char *buffer, dtpreg_t *dtp);

extern uint8_t tape_end(dtpreg_t *dtp);

extern uint8_t tape_eof(dtpreg_t *dtp);

extern uint8_t tape_eob(dtpreg_t *dtp);

#endif
