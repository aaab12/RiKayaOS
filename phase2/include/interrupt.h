#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "const.h"
#include "const_rikaya.h"
#include <umps/libumps.h>
#include "main.h"
#include "scheduler.h"
#include "types_rikaya.h"
#include "utils.h"

void plt_handler();

void it_handler();

void disk_handler();

void tape_handler();

void network_handler();

void printer_handler();

void terminal_handler();

#endif
