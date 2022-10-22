#include "../lib.h"
#include "../synch.h"
#include "../idt.h"
#include "../i8259.h"
#include "keyboard.h"

#define KB_DATAPORT 0x60
#define KB_IRQ 0x1
#define RETURN_CODE 
#define BACKSPACE_CODE 0x0e // scan code for backspace pressed
#define ENTER_CODE 0x1c // scan code for enter pressed

char scanTable[0xff];
char * buf; // the buffer to write characters to
int length; // length of buf
int pos; // position in buf to write to next (0 indexed)
int reading; // flag that says whether or not keyboard is currently in a read

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

/* gets
 * Inputs: buffer - the buffer to write to 
 *         nbytes - the number of bytes or characters to write to buffer
 * Outputs: buffer - the data in this buffer is overwritten by characters
 *          read from the keyboard, ending with a line feed where the user presses enter
 * Return Value: the number of bytes or characters written to buffer
 */
int32_t gets(char * buffer, int nbytes){
    if (nbytes < 2)
        return -1;
    buf = buffer;
    length = nbytes-1;
    pos = 0;
    reading = 1;
    enable_irq(KB_IRQ);
    while (reading){}
    disable_irq(KB_IRQ);
    return pos;
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
    uint8_t input = inb(KB_DATAPORT);
    disable_irq(KB_IRQ);
    if (reading){
        if (input == ENTER_CODE){
            reading = 0;
            buf[pos] = '\n';
        } else if (input == BACKSPACE_CODE){
            if (pos > 0){
                buf[pos-1] = '_';
                pos--;
            }
        } else if (pos < length){
            if (((input <= 0x0b) && (input >= 0x02)) ||  //checking bounds of the scan code according to init above
                ((input <= 0x19) && (input >= 0x10)) ||
                ((input <= 0x26) && (input >= 0x1e)) ||
                ((input <= 0x32) && (input >= 0x2c))){
                buf[pos] = scanTable[input]; // add character to buffer we're currently writing to
                putc(scanTable[input]);
                pos++;
            }
        }
    }
    send_eoi(KB_IRQ);
    enable_irq(KB_IRQ);
    sti();
    
    asm volatile(" \n\
    leave \n\
    iret"
    );
}

