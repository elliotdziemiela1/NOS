#include "../lib.h"
#include "../synch.h"
#include "../idt.h"
#include "rtc.h"
#include "../tests.h"

#define RTC_IRQ 0x8
#define RTC_INDEX_PORT 0x70

void rtc_init(){ // MAKE SURE TO INSTALL RTC HANDLER BEFORE CALLING THIS FUNCTION
    outb(RTC_INDEX_PORT, 0x8B);		// x8B: select register B, and disable NMI
    char prev= inb(RTC_INDEX_PORT+1);	// x71: read the current value of register B
    outb(RTC_INDEX_PORT, 0x8B);		// set the index again (a read will reset the index to register D)
    outb(RTC_INDEX_PORT+1, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    insert_handler(RTC_IRQ, &rtc_handler,0);
}

void rtc_handler(){
    printf("rtc is working");
    test_interrupts();
    outb(RTC_INDEX_PORT,0x0C);
    inb(0x71);
}
