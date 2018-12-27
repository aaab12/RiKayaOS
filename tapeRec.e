#ifndef TAPEREC_E
#define TAPEREC_E

extern uint32_t tapeReadBlock(char *buffer, dtpreg_t *dtp);

extern void tapeSkipBlock(dtpreg_t *dtp);

extern uint8_t tape_End(dtpreg_t *dtp);

#endif
