/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */



/* Initialize the 8259 PIC */
void i8259_init(void) {
    // +1 means data port. xff is a full mask of the PICs' IRs
    master_mask = 0xff;
    slave_mask = 0xff;

    outb(master_mask, MASTER_8259_PORT+1); // mask all of PIC 1
    outb(slave_mask, SLAVE_8259_PORT+1); // mask all of PIC 2

    outb(ICW1, MASTER_8259_PORT); // tells PIC 1 to expect initialization words
    outb(ICW2_MASTER, MASTER_8259_PORT+1); // tells PIC 1 where to map IR0-7 (maps to x20-x27 in IDT)
    outb(ICW3_MASTER, MASTER_8259_PORT+1); // tells PIC 1 that has a secondary PIC on IRQ2
    outb(ICW4, MASTER_8259_PORT+1);

    outb(ICW1, SLAVE_8259_PORT); // tells PIC 2 to expect initialization words
    outb(ICW2_SLAVE, SLAVE_8259_PORT+1); // tells PIC 2 where to map IR0-7 (maps to x28-x2f in IDT)
    outb(ICW3_SLAVE, SLAVE_8259_PORT+1); // tells PIC 2 that it is a secondary PIC on IRQ2
    outb(ICW4, SLAVE_8259_PORT+1);

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
}
