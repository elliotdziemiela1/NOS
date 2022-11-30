#include "pit.h"
#include "../idt.h"
#include "../i8259.h"
#include "../lib.h"
#include "rtc.h"

volatile uint32_t pit_timer = 0;

void pit_init(){
    insert_handler(PIT_IRQ, (int) &pit_handler, 0); //0 for dpl 0

    outb(PIT_MODE, PIT_REG_CMD);
    outb((uint8_t) PIT_INTERVAL, PIT_REG_DATA);
    outb((uint8_t) (PIT_INTERVAL << 8), PIT_REG_DATA);

    enable_irq(PIT_IRQ);
}

void pit_handler(){
    cli();
    pit_timer++;
    send_eoi(PIT_IRQ);
    //Context switch into next terminal
}
