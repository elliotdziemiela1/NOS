#include "../lib.h"
#include "../synch.h"
#include "../idt.h"
#include "rtc.h"
#include "../tests.h"
#include "../i8259.h"

#define RTC_IRQ 0x8
#define RTC_INDEX_PORT 0x70

char prev;
unsigned int rate;

/* rtc_init
 * 
 * Initializes the RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: Set up the RTC
 * Coverage: RTC Initialization
 * Files: None
 */
void rtc_init(){ // MAKE SURE TO INSTALL RTC HANDLER BEFORE CALLING THIS FUNCTION


    outb(0x8B, RTC_INDEX_PORT);		// x8B: select register B, and disable NMI
    prev= inb(RTC_INDEX_PORT+1);	// x71: read the current value of register B
    outb(0x8B, RTC_INDEX_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_INDEX_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B


    rate &= 0;
    rate = 6;
   
    // outb(0x8A, RTC_INDEX_PORT);		// x8B: select register B, and disable NMI
    // prev= inb(RTC_INDEX_PORT+1);	// x71: read the current value of register B
    // outb(0x8A, RTC_INDEX_PORT);		// set the index again (a read will reset the index to register D)
    // outb((prev & 0xF0) | rate, RTC_INDEX_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

    insert_handler(RTC_IRQ, &rtc_handler,0);
    enable_irq(RTC_IRQ);

}

/* rtc_handler
 * 
 * Function that acknowledges the interrupt and sends eoi to rtc
 * Inputs: None
 * Outputs: None
 * Side Effects: Sends EOI to RTC
 * Coverage: RTC Interrupt Handling
 * Files: None
 */
void rtc_handler(){

    disable_irq(RTC_IRQ);
    outb(0x0C, RTC_INDEX_PORT);
    inb(0x71);
    printf("rtc is working");
    test_interrupts();
    

    send_eoi(RTC_IRQ);

    enable_irq(RTC_IRQ);

}
