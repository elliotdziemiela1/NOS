#include "../lib.h"
#include "../synch.h"
#include "../idt.h"
#include "../i8259.h"
#include "keyboard.h"
#include "scheduling.h"

#define KB_DATAPORT 0x60
#define KB_IRQ 0x1
#define BACKSPACE_CODE 0x0e  // scan codes
#define ENTER_CODE 0x1c 
#define CAPSLOCK_CODE 0x3a
#define LEFT_SHIFT_PRESSED_CODE 0x2a
#define LEFT_SHIFT_RELEASED_CODE 0xaa
#define RIGHT_SHIFT_PRESSED_CODE 0x36
#define RIGHT_SHIFT_RELEASED_CODE 0xb6
#define CTRL_PRESSED_CODE 0x1d
#define CTRL_RELEASED_CODE 0x9d
#define L_CODE 0x26

char scanTable[0x40] = {'\0','\0','1','2','3','4','5','6','7','8','9','0','-',
    '=','\0','\0','q','w','e','r','t','y','u','i','o','p','[',']','\0','\0','a',
    's','d','f','g','h','j','k','l',';','\'','`','\0','\\','z','x','c','v',
    'b','n','m',',','.','/','\0','\0','\0',' '};
char scanTableShift[0x40] = {'\0','\0','!','@','#','$','%','^','&','*','(',')','_',
    '+','\0','\0','Q','W','E','R','T','Y','U','I','O','P','{','}','\0','\0','A',
    'S','D','F','G','H','J','K','L',':','\"','~','\0','|','Z','X','C','V',
    'B','N','M','<','>','?','\0','\0','\0',' '};
char scanTableCapsLock[0x40] = {'\0','\0','1','2','3','4','5','6','7','8','9','0','-',
    '=','\0','\0','Q','W','E','R','T','Y','U','I','O','P','[',']','\0','\0','A',
    'S','D','F','G','H','J','K','L',';','\'','`','\0','\\','Z','X','C','V',
    'B','N','M',',','.','/','\0','\0','\0',' '};


static char * buf1; // the buffer to write characters to of the first terminal
static char * buf2; // the buffer to write characters to of the second terminal
static char * buf3; // the buffer to write characters to of the third terminal
static int length; // length of buf
static int pos; // position in buf to write to next (0 indexed)
static int reading; // flag that says whether or not keyboard is currently in a read
static int shift; // flag that says whether or not shift is being held
static int capsLock; // flag that says whether or not caps lock is on
static int ctrl; // flag that says whether or not ctrl is pressed

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
}

/* gets
 * Inputs: buffer - the buffer to write to 
 *         nbytes - the number of bytes or characters to write to buffer
 * Outputs: buffer - the data in this buffer is overwritten by characters
 *          read from the keyboard, ending with a line feed where the user presses enter
 * Return Value: the number of bytes or characters written to buffer
 */
int32_t gets(char * buffer, int nbytes){
    if (nbytes < 1) // shoulnt be called with 0 bytes
        return 0;
    if (current_terminal == 1){
        buf1 = buffer;
    } if (current_terminal == 2){
        buf2 = buffer;
    } if (current_terminal == 3){
        buf3 = buffer;
    }
    length = nbytes-1;
    pos = 0;
    reading = 1;
    shift = 0;
    capsLock = 0;
    ctrl = 0;
    enable_irq(KB_IRQ);
    while (reading){} // this will end when the user presses enter in the keyboard
    // handler, setting reading to false.
    disable_irq(KB_IRQ);
    return pos;
}

static void addToBuffer(int index, char c){
    if (current_terminal == 1){
        buf1[index] = c;
    } if (current_terminal == 2){
        buf2[index] = c;
    } if (current_terminal == 3){
        buf3[index] = c;
    }
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
    if (current pid is not in printing terminal)
        return 0;
    cli();
    uint8_t input = inb(KB_DATAPORT);
    disable_irq(KB_IRQ);
    if (reading){
        if (input == ENTER_CODE){
            reading = 0;
            addToBuffer(pos,'\0');
            // putcBetter('\n');
        } else if (input == BACKSPACE_CODE){
            if (pos > 0){
                addToBuffer(pos-1,' ');
                pos--;
                setCursor(getCursorX()-1,getCursorY());
                putcBetter(' ');
                setCursor(getCursorX()-1,getCursorY());
            }
        } else if (input == CAPSLOCK_CODE){
            capsLock ^= 1; // flips the status of capsLock
        } else if ((input==LEFT_SHIFT_PRESSED_CODE) || (input==RIGHT_SHIFT_PRESSED_CODE)){
            shift = 1;
        } else if ((input==LEFT_SHIFT_RELEASED_CODE) || (input==RIGHT_SHIFT_RELEASED_CODE)){
            shift = 0;
        } else if (input==CTRL_PRESSED_CODE){
            ctrl = 1;
        } else if (input==CTRL_RELEASED_CODE){
            ctrl = 0;
        } else if ((pos<length) && (input<=0x39)){ // x39 is the last index in the scan code arrays
            if (ctrl){
                if (input == L_CODE){ // ctrl + l = clear screen and reset cursor to top left
                    clear();
                    setCursor(0,0);
                }
            } else if (shift){
                addToBuffer(pos,scanTableShift[input]);// add character to buffer we're currently writing to
                putcBetter(scanTableShift[input]);
            } else if (capsLock){
                addToBuffer(pos,scanTableCapsLock[input]);// add character to buffer we're currently writing to
                putcBetter(scanTableCapsLock[input]);
            } else {
                addToBuffer(pos,scanTable[input]);// add character to buffer we're currently writing to
                putcBetter(scanTable[input]);
            }
            pos++;
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

