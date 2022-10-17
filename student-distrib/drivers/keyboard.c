#include "../lib.h"
#include "../synch.h"
#include "../idt.h"
#include "../i8259.h"
#include "keyboard.h"

#define KB_DATAPORT 0x60
#define KB_IRQ 0x1

char scanTable[0xff];
/* keyboard_init
 * 
 * Initializes the keyboard
 * Inputs: None
 * Outputs: None
 * Side Effects: Sets up the initialization of keyboard
 * Coverage: Keyboard Initialization
 * Files: None
 */
void keyboard_init(){
    insert_handler(KB_IRQ, (int)&keyboard_handler,0);
    int i;
    for (i = 0; i < 0xff; i++){ //0xFF for 256 entries
        scanTable[i] = '!';
    }
    //numbers
    scanTable[0x02] = '1';
    scanTable[0x03] = '2';
    scanTable[0x04] = '3';
    scanTable[0x05] = '4';
    scanTable[0x06] = '5';
    scanTable[0x07] = '6';
    scanTable[0x08] = '7';
    scanTable[0x09] = '8';
    scanTable[0x0a] = '9';
    scanTable[0x0b] = '0';
    //first letter line
    scanTable[0x10] = 'q';
    scanTable[0x11] = 'w';
    scanTable[0x12] = 'e';
    scanTable[0x13] = 'r';
    scanTable[0x14] = 't';
    scanTable[0x15] = 'y';
    scanTable[0x16] = 'u';
    scanTable[0x17] = 'i';
    scanTable[0x18] = 'o';
    scanTable[0x19] = 'p';
    //second letter line
    scanTable[0x1e] = 'a';
    scanTable[0x1f] = 's';
    scanTable[0x20] = 'd';
    scanTable[0x21] = 'f';
    scanTable[0x22] = 'g';
    scanTable[0x23] = 'h';
    scanTable[0x24] = 'j';
    scanTable[0x25] = 'k';
    scanTable[0x26] = 'l';
    // last letter line
    scanTable[0x2c] = 'z';
    scanTable[0x2d] = 'x';
    scanTable[0x2e] = 'c';
    scanTable[0x2f] = 'v';
    scanTable[0x30] = 'b';
    scanTable[0x31] = 'n';
    scanTable[0x32] = 'm';
}


/* keyboard_handler
 * 
 * Handles the keyboard interrupts
 * Inputs: None
 * Outputs: None
 * Side Effects: Handles the keyboard interrupts
 * Coverage: Keyboard interrupt handling
 * Files: None
 */
void keyboard_handler(){
    cli();
    uint8_t tmp = inb(KB_DATAPORT);
    disable_irq(KB_IRQ);
    if (((tmp <= 0x0b) && (tmp >= 0x02)) ||  //checking bounds of the scan code according to init above
        ((tmp <= 0x19) && (tmp >= 0x10)) ||
        ((tmp <= 0x26) && (tmp >= 0x1e)) ||
        ((tmp <= 0x32) && (tmp >= 0x2c))){
        putc(scanTable[tmp]);
    }
    send_eoi(KB_IRQ);
    enable_irq(KB_IRQ);
    sti();
}

