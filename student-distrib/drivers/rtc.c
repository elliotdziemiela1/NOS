#include "../lib.h"
#include "../synch.h"
#include "../idt.h"
#include "rtc.h"
#include "../tests.h"
#include "../i8259.h"
#include "terminal.h"


#define RTC_INDEX_PORT 0x70
#define MAX_FREQ 1024
#define MIN_FREQ 2
#define RTC_FREQ 1024

char prev;
unsigned int rate;

int counters[3] = {0,0,0};
int flags[3] = {0,0,0};

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

    outb(0x0C, RTC_INDEX_PORT); // x0c: select register A, and disable NMI
    inb(0x71); // x71: read the current value of register B

    counters[current_terminal_executing]++; // increment counter for the executing terminal
    if(counters[current_terminal_executing] >= terminals[current_terminal_executing].rtc_mod){ // checks if executing terminal's rtc should raise a flag
        flags[current_terminal_executing] = 1; // raising terminal's rtc flag
        counters[current_terminal_executing] = 0; // setting rtc counter back to zero
    }
    send_eoi(RTC_IRQ); // send end of interrupt
    
    // returning from handler
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
    
    flags[current_terminal_executing] = 0; // makes sure flag is initially zero

    // spinlock around executing terminal's rtc flag
    while(1){
        if(flags[current_terminal_executing]){
            break; // break and return once rtc interrupt is raised
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

    int32_t freq =*((int32_t*)buf); // string user program's frequency

    // altering terminal rtc mod value by a constant factor (5.5) determined through experimentation
    terminals[current_terminal_executing].rtc_mod = (RTC_FREQ/(freq*5.5));

    return 0; //return success
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
        return -1; // return failure if empty filename is passed
    }
    return current_terminal_displaying; // return current terminal displaying as the file descriptor index
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
    return 0; // rtc cannot be closed so simply return 0
}

