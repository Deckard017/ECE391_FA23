#ifndef _PIT_H
#define _PIT_H

#define PIT_FREQUENCY 11931
#define MODE 0x36
#define PORT_FOR_DATA 0x40
#define PORT_FOR_MODE 0x43
#define PIT_IRQ 0

extern void init_pit();
extern void handler_pit();

#endif
