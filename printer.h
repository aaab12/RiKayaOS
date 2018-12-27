#ifndef PRINTER_H
#define PRINTER_H

#include "umps/libumps.h"
#include "umps/arch.h"
#include "umps/types.h"
#include "stdint.h"

#define ST_READY           1
#define ST_BUSY            3

#define CMD_ACK            1
#define CMD_PRINTCHR       2


#define PRINTER_STATUS_MASK   0xFF

#endif
