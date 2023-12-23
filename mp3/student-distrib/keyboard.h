#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "terminal.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT 0x60


/*Initialize funciton*/
extern void init_keyboard();
/*handle function*/
extern void handle_keyboard();
#endif
