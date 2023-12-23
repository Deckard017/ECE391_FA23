#include "pit.h"
#include "schedule.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"
/*
* initailize the pit
*/
void init_pit()
{
    outb(MODE, PORT_FOR_MODE);
    outb(PIT_FREQUENCY & 0x00ff, PORT_FOR_DATA);
    outb((PIT_FREQUENCY  & 0xff00)>>8, PORT_FOR_DATA);
    enable_irq(PIT_IRQ);
}
/*
* handle the pit
*/
void handler_pit()
{
    send_eoi(PIT_IRQ);
    //cli();
    schedule_init();
    //sti();
    return;
}
