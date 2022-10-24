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
int flag;

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
   
    outb(0x8A, RTC_INDEX_PORT);		// x8B: select register B, and disable NMI
    prev= inb(RTC_INDEX_PORT+1);	// x71: read the current value of register B
    outb(0x8A, RTC_INDEX_PORT);		// set the index again (a read will reset the index to register D)
    outb((prev & 0xF0) | rate, RTC_INDEX_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

    insert_handler(RTC_IRQ, (int)&rtc_handler,0);
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

    outb(0x0C, RTC_INDEX_PORT);
    inb(0x71);
    flag = 1;
    test_interrupts();
    send_eoi(RTC_IRQ);
    
    asm volatile(" \n\
    leave \n\
    iret"
    );

}

int32_t read(int32_t fd, void* buf, int32_t nbytes){
//check to see if interrupt has been generated and set flag accordingly
    flag = 0;
    while(1){
        if(flag){
            break;
        }
    return 0;
        }
    }
        
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    //check if all values passed in are valid
    if(fd == 0 || fd == 1|| buf == NULL || nbytes != 4 ) {
		return -1;
	}

    int32_t freq =*((int32_t*)buf);
    if(set_frequency(freq) != 0){
        return -1;
    }
    set_frequency(freq);

    return nbytes; //discussion slides say to return 0 or -1 but appendix b says number of bytes??
}

int32_t open(const uint8_t* filename){
    if(filename == NULL){
        return -1;
    }
    set_frequency(2);
    return 0;
}

int32_t close(int32_t fd){
    return 0;
}

int32_t set_frequency(int32_t freq){
    if (freq < 2 || freq > 1024){ //check if frequency is greater than minimum hz and less than max hz
        return -1;
    }
    if(is_power_of_two(freq) == 0){
        return -1;
    }

    switch(freq){
        case 2: 
            rate= 0x0F;
        case 4: 
            rate= 0x0E;
        case 8: 
            rate= 0x0D;
        case 16: 
            rate= 0x0C;
        case 32: 
            rate= 0x0B;    
        case 64: 
            rate= 0x0A;
        case 128: 
            rate= 0x09;
        case 256: 
            rate= 0x08;
        case 512: 
            rate= 0x07;
        case 1024: 
            rate= 0x06;
        default: 
            return -1;
    }

    outb(0x8A, RTC_INDEX_PORT);		// x8B: select register B, and disable NMI
    prev= inb(RTC_INDEX_PORT+1);	// x71: read the current value of register B
    outb(0x8A, RTC_INDEX_PORT);		// set the index again (a read will reset the index to register D)
    outb((prev & 0xF0) | rate, RTC_INDEX_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

    insert_handler(RTC_IRQ, (int)&rtc_handler,0);
    enable_irq(RTC_IRQ);
    return 0;

}

int is_power_of_two(int num){
    //check to see if number is power of 2
    if(num == 1 || num == 0){
        return 0;
    }
    while(num!=1){
        num = num/2;
        if((n%2 != 0) && (num !- 1)){
            return 0;
        }
    return 1;
    }

}
