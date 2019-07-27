#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#define TERM_ST_BUSY         3
#define TERM_ST_TRANSMITTED  5
#define TERM_STATUS_MASK     0xFF

void plt_handler();

void it_handler();

void disk_handler();

void tape_handler();

void network_handler();

void printer_handler();

void terminal_handler();

#endif
