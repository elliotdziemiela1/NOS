#include "../lib.h"
#include "../synch.h"
#include "../idt.h"
#include "rtc.h"
#include "../tests.h"
#include "../i8259.h"
#include "terminal.h"

#define RTC_IRQ 0x8
#define RTC_INDEX_PORT 0x70
#define MAX_FREQ 1024
#define MIN_FREQ 2
#define RTC_FREQ 1024

char prev;
unsigned int rate;
int flag;

int counters[3] = {0,0,0};
int flags[3] = {0,0,0};

//Note: used OSDEV for init, handler

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
    insert_handler(RTC_IRQ, (int)&rtc_handler,0);


    outb(0x8B, RTC_INDEX_PORT);		// x8B: select register B, and disable NMI
    prev= inb(RTC_INDEX_PORT+1);	// x71: read the current value of register B
    outb(0x8B, RTC_INDEX_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_INDEX_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B


    rate &= 0;
    rate = 0x06; //default rate where frequency = 1024
   
    outb(0x8A, RTC_INDEX_PORT);		// x8B: select register B, and disable NMI
    prev= inb(RTC_INDEX_PORT+1);	// x71: read the current value of register B
    outb(0x8A, RTC_INDEX_PORT);		// set the index again (a read will reset the index to register D)
    outb((prev & 0xF0) | rate, RTC_INDEX_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

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
    // flag = 1;
    counters[current_terminal_executing]++;
    if(counters[current_terminal_executing] >= terminals[current_terminal_executing].rtc_mod){
        flags[current_terminal_executing] = 1;
        counters[current_terminal_executing] = 0;
    }
    //test_interrupts();
    send_eoi(RTC_IRQ);
    
    asm volatile(" \n\
    leave \n\
    iret"
    );

}

/* rtc_read
 * 
 * Function that blocks until the next interrupt
 * Inputs: file descriptor, buffer to read, number of bytes to read
 * Outputs: None
 * Return value: 0
 * Files: None
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
//check to see if interrupt has been generated and set flag accordingly
    // flag = 0;
    flags[current_terminal_executing] = 0;
    while(1){
        if(flags[current_terminal_executing]){
            break;
        }
    }
    return 0;
}

/* rtc_write
 * 
 * Function changes frequency and writes a file 
 * Inputs: file descriptor, buffer to read, number of bytes to read
 * Outputs: None
 * Return value: -1 if fail and 0 if success
 * Files: None
 */    
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    // printf("REACHED RTC_WRITE \n");
    //check if all values passed in are valid
    // if(fd == 0 || fd == 1|| buf == NULL || nbytes != 4 ) {
	// 	return -1;
	// }

    int32_t freq =*((int32_t*)buf);

    terminals[current_terminal_executing].rtc_mod = (RTC_FREQ/(freq*5.5));

    // if(set_frequency(freq) != 0){
    //     return -1;
    // }
    // set_frequency(freq);

    return 0; //discussion slides say to return 0 or -1 but appendix b says number of bytes??
}

/* rtc_open
 * 
 * Function opens file and sets default frequency to 2 Hz
 * Inputs: file descriptor, buffer to read, number of bytes to read
 * Outputs: None
 * Return value: -1 if fail and 0 if success
 */
int32_t rtc_open(const uint8_t* filename){
    if(filename == NULL){
        return -1;
    }
    // set_frequency(2);
    return current_terminal_displaying;
}

/* rtc_close
 *
 * Function closes a file
 * Inputs: file descriptor, buffer to read, number of bytes to read
 * Outputs: None
 * Return value: 0
 * Files: None
 */
int32_t rtc_close(int32_t fd){
    return 0;
}

/* set_frequency
 * 
 * Function takes frequency and changes the rate based on the value
 * Inputs: frequency
 * Outputs: None
 * Return value: -1 if fail and 0 if success
 * Files: None
 */
// int32_t set_frequency(int32_t freq){
//     if (freq < MIN_FREQ || freq > MAX_FREQ){ //check if frequency is greater than minimum hz and less than max hz
//         return -1;
//     }
//     if(is_power_of_two(freq) == 0){
//         return -1;
//     }
//     //choose rate based on frequency value
//     switch(freq){
//         case 2: 
//             rate= 0x0F;
//             break;
//         case 4: 
//             rate= 0x0E;
//             break;
//         case 8: 
//             rate= 0x0D;
//             break;
//         case 16: 
//             rate= 0x0C;
//             break;
//         case 32: 
//             rate= 0x0B;
//             break;    
//         case 64: 
//             rate= 0x0A;
//             break;
//         case 128: 
//             rate= 0x09;
//             break;
//         case 256: 
//             rate= 0x08;
//             break;
//         case 512: 
//             rate= 0x07;
//             break;
//         case 1024: 
//             rate= 0x06;
//             break;
//         default: 
//             return -1;
//     }

//     outb(0x8A, RTC_INDEX_PORT);		// x8B: select register B, and disable NMI
//     prev= inb(RTC_INDEX_PORT+1);	// x71: read the current value of register B
//     outb(0x8A, RTC_INDEX_PORT);		// set the index again (a read will reset the index to register D)
//     outb((prev & 0xF0) | rate, RTC_INDEX_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

//     insert_handler(RTC_IRQ, (int)&rtc_handler,0);
//     enable_irq(RTC_IRQ);
//     return 0;

// }

/* is_power_of_two
 * 
 * Function checks if value is power of 2
 * Inputs: number to check
 * Outputs: None
 * Return value: 1 if success and 0 if fail
 * Files: None
 */
int is_power_of_two(int num){
    //check to see if number is power of 2
    if(num == 1 || num == 0){
        return 0;
    }
    while(num!=1){
        num = num/2;
        if((num%2 != 0) && (num != 1)){
            return 0;
        }
    }
    return 1;
}
