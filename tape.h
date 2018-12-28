#ifndef TAPE_H
#define TAPE_H

#define MK_EOT			  0
#define MK_EOF			  1
#define MK_EOB			  2
#define MK_TS  			  3

#define ST_READY		  1
#define ST_BUSY			  3

#define CMD_ACK			  1
#define CMD_RESET		  0
#define CMD_SKIPBLK		2
#define CMD_READBLK		3
#define CMD_BACKBLK		4

#endif
