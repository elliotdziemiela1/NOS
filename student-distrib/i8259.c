/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#include "synch.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* spin lock for pic data */
spin_lock pic_lock;



/* Initialize the 8259 PIC */
void i8259_init(void) {
    pic_lock = new_lock();
    unsigned long flags;
    spin_lock_irsave(&pic_lock, &flags);

    master_mask = 0xff; 
    slave_mask = 0xff;

    outb(master_mask, MASTER_8259_PORT+1); // mask all of PIC 1. +1 means data port. 
    outb(slave_mask, SLAVE_8259_PORT+1); // mask all of PIC 2

    outb(ICW1, MASTER_8259_PORT); // tells PIC 1 to expect initialization words
    outb(ICW2_MASTER, MASTER_8259_PORT+1); // tells PIC 1 where to map IR0-7 (maps to x20-x27 in IDT)
    outb(ICW3_MASTER, MASTER_8259_PORT+1); // tells PIC 1 that has a secondary PIC on IRQ2
    outb(ICW4, MASTER_8259_PORT+1);

    outb(ICW1, SLAVE_8259_PORT); // tells PIC 2 to expect initialization words
    outb(ICW2_SLAVE, SLAVE_8259_PORT+1); // tells PIC 2 where to map IR0-7 (maps to x28-x2f in IDT)
    outb(ICW3_SLAVE, SLAVE_8259_PORT+1); // tells PIC 2 that it is a secondary PIC on IRQ2
    outb(ICW4, SLAVE_8259_PORT+1);

    // Don't know about udelay and outputting the cached values.
    spin_unlock_irrestore(&pic_lock, flags);

}

/* Enable (unmask) the specified IRQ by setting it's bit in the mask to 0*/
void enable_irq(uint32_t irq_num) {
    uint8_t irq = 1;
    int i;

    if (irq_num <= 7){ // irq lies on pic 1 (IRQs 0-7)
        for (i = 0; i < irq_num; i++;)
            irq = irq<<1;
        irq ^= irq; // flips bits of irq (should now be seven 1s and one 0)
        master_mask &= irq;
    } else if (irq_num <= 15){ // irq lies on pic 2 (IRQs 8-15)
        for (i = 0; i < irq_num-8; i++;)
            irq = irq<<1;
        irq ^= irq; // flips bits of irq (should now be seven 1s and one 0)
        slave_mask &= irq;
    }
    // could write code to raise exception if >15 or if irq is already enabled
}

/* Disable (mask) the specified IRQ by setting it's bit in the mask to 1*/
void disable_irq(uint32_t irq_num) {
    uint8_t irq = 1;
    int i;

    if (irq_num <= 7){ // irq lies on pic 1 (IRQs 0-7)
        for (i = 0; i < irq_num; i++;)
            irq = irq<<1;
        master_mask |= irq;
    } else if (irq_num <= 15){ // irq lies on pic 2 (IRQs 8-15)
        for (i = 0; i < irq_num-8; i++;)
            irq = irq<<1;
        slave_mask |= irq;
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
}
