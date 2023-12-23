/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "rtc.h"
#include "keyboard.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    outb(0xFF,MASTER_8259_PORT2);
    outb(0xFF,SLAVE_8259_PORT2);

    outb(ICW1,MASTER_8259_PORT);
    outb(ICW2_MASTER,MASTER_8259_PORT2);
    outb(ICW3_MASTER,MASTER_8259_PORT2);
    outb(ICW4,MASTER_8259_PORT2);

    outb(ICW1,SLAVE_8259_PORT);
    outb(ICW2_SLAVE,SLAVE_8259_PORT2);
    outb(ICW3_SLAVE,SLAVE_8259_PORT2);
    outb(ICW4,SLAVE_8259_PORT2);

    master_mask = 0xFF;
    slave_mask = 0xFF;
    enable_irq(2);     //enable slave irq
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    if(irq_num<8)
    {       //master
        master_mask &= ~(1<<irq_num);   //set the corresponding bit to 0
        outb(master_mask,MASTER_8259_PORT+1);
    }
    else
    {       //slave
        slave_mask &= ~(1<<(irq_num-8));
        outb(slave_mask,SLAVE_8259_PORT+1);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if(irq_num<8)
    {       //master
        master_mask |= (1<<irq_num);
        outb(master_mask,MASTER_8259_PORT+1);
    }
    else
    {       //slave
        slave_mask |= (1<<(irq_num-8));
        outb(slave_mask,SLAVE_8259_PORT+1);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num<8)
    {       //master
        outb(EOI|irq_num,MASTER_8259_PORT);
    }
    else
    {       //slave
        outb(EOI|(irq_num-8),SLAVE_8259_PORT);
        outb(EOI|2,MASTER_8259_PORT);
    }
}
