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
//No inputs or outputs
//Side effects is that it initialzies the PIC
void i8259_init(void) {
    pic_lock = new_lock();

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

    enable_irq(2);

}

/* Enable (unmask) the specified IRQ by setting it's bit in the mask to 0*/
/* Masking and unmasking of interrupts on an 8259A outside of the interrupt 
* sequence requires only a single write to the second port (0x21 or 0xA1), 
* and the byte written to this port specifies which interrupts should be masked. 
* - course notes
*/
/* I dont think I need to acquire the lock for enable/disable since data writing is atomic*/
void enable_irq(uint32_t irq_num) {
    uint8_t irq = 1;
    int i;
    if (irq_num < 0)
        return;
    if (irq_num <= 7){ // irq lies on pic 1 (IRQs 0-7)
        for (i = 0; i < irq_num; i++)
            irq = irq<<1;
        master_mask ^= irq; 
        outb(master_mask, MASTER_8259_PORT+1); // mask all of PIC 1. +1 means data port. 
    } else if (irq_num <= 15){ // irq lies on pic 2 (IRQs 8-15)
        for (i = 0; i < irq_num-8; i++)
            irq = irq<<1;
        irq ^= irq; // flips bits of irq (should now be seven 1s and one 0)
        slave_mask &= irq;
        outb(slave_mask, SLAVE_8259_PORT+1); // mask all of PIC 2. +1 means data port.
    }
}

/* Disable (mask) the specified IRQ by setting it's bit in the mask to 1*/
void disable_irq(uint32_t irq_num) {
    uint8_t irq = 1;
    int i;

    if (irq_num <= 7){ // irq lies on pic 1 (IRQs 0-7)
        // for (i = 0; i < irq_num; i++)
        //     irq = irq<<1;
        master_mask |= (1<<irq_num);
        outb(master_mask, MASTER_8259_PORT+1); // mask all of PIC 1. +1 means data port. 
    } else if (irq_num <= 15){ // irq lies on pic 2 (IRQs 8-15)
        // for (i = 0; i < irq_num-8; i++) //go to next irq number
        //     irq = irq<<1;
        slave_mask |= (1<<(irq_num-8));
        outb(slave_mask, SLAVE_8259_PORT+1); // mask all of PIC 2. +1 means data port.
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num > 16){
        return;
    }
    if (irq_num < 8){
        outb(EOI|irq_num, MASTER_8259_PORT);
    } else if (irq_num < 16){
        irq_num = irq_num - 8;
        outb(EOI | irq_num, SLAVE_8259_PORT);
        outb(EOI | 2, MASTER_8259_PORT);
    }

    // if(irq_num >= 8)
    //     outb(SLAVE_8259_PORT, EOI|irq_num);
    // outb(MASTER_8259_PORT, EOI|irq_num);
    
}
