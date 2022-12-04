#include "../lib.h"
#include "../synch.h"
#include "../idt.h"
#include "../i8259.h"
#include "keyboard.h"
#include "../scheduling.h"
#include "terminal.h"
#include "pit.h"

#define KB_DATAPORT 0x60
#define BACKSPACE_CODE 0x0e  // scan codes
#define ENTER_CODE 0x1c 
#define CAPSLOCK_CODE 0x3a
#define LEFT_SHIFT_PRESSED_CODE 0x2a
#define LEFT_SHIFT_RELEASED_CODE 0xaa
#define RIGHT_SHIFT_PRESSED_CODE 0x36
#define RIGHT_SHIFT_RELEASED_CODE 0xb6
#define CTRL_PRESSED_CODE 0x1d
#define CTRL_RELEASED_CODE 0x9d
#define ALT_PRESSED_CODE 0x38
#define ALT_RELEASED_CODE 0xb8
#define L_CODE 0x26
#define F1_CODE 0x3b
#define F2_CODE 0x3c
#define F3_CODE 0x3d

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

// ONLY 3 BUFFERS, EACH CORRESPONDING TO THE BUFFER OF THE TOPMOST SHELL IN EACH TERMINAL.
static char * buf1; // the buffer to write characters to of the first terminal
static char * buf2; // the buffer to write characters to of the second terminal
static char * buf3; // the buffer to write characters to of the third terminal
static int length; // length of buf
static int shift; // flag that says whether or not shift is being held
static int capsLock; // flag that says whether or not caps lock is on
static int ctrl; // flag that says whether or not ctrl is pressed
static int alt; // flag that says whether or not alt is pressed

/* static void clear_buffer;
 * Inputs: - int num - indicator of buffer we want to clear
 * Return Value: None
 * Function: Clears the keyboard buffer for the argument that we pass in
 * */
static void clearBuffer(int num){
    int i = 0;
    for (i = 0; i < BUFFER_SIZE; i++){
        if (num == 0)
            *(buf1+i) = '\0';
        if (num == 1)
            *(buf1+i) = '\0';
        if (num == 2)
            *(buf1+i) = '\0';
    }
}

/* static void addToBuffer;
 * Inputs: - int index: place in buffer we want to add a character, char c: character that we want to add
 * Return Value: None
 * Function: Adds a character to the corresponding buffer
 * */
static void addToBuffer(int index, char c){ // could be synchronization issues
    // buf1[index] = c; // SO FAR WE ONLY HAVE 1 SHELL EXECUTING THUS ONLY 1 BUFFER
    if (current_terminal_displaying == 0){
        buf1[index] = c;
    } if (current_terminal_displaying == 1){
        buf2[index] = c;
    } if (current_terminal_displaying == 2){
        buf3[index] = c;
    }
}
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
    enable_irq(KB_IRQ);
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
    if (current_terminal_displaying == 0){
        buf1 = terminals[current_terminal_executing].keyboard_buffer;
    } if (current_terminal_displaying == 1){
        buf2 = terminals[current_terminal_executing].keyboard_buffer;
    } if (current_terminal_displaying == 2){
        buf3 = terminals[current_terminal_executing].keyboard_buffer;
    }
    length = nbytes-1;
    terminals[current_terminal_executing].kb_buffer_position = 0;
    terminals[current_terminal_executing].reading = 1;
    shift = 0;
    capsLock = 0;
    ctrl = 0;
    // enable_irq(KB_IRQ);

    // clears input buffer
    int i = 0;
    for (i = 0; i < BUFFER_SIZE; i++){
        *(buffer+i) = '\0';
    }
    while (terminals[current_terminal_executing].reading){} // this will end when the user presses enter in the keyboard
    // handler, setting reading to false. If we call one instance of this function in one terminal, then switch to another 
    // executing terminal, the while loop wont end. It only ends when we are displaying the terminal that called gets
    // and press enter. This loop will however end until we start doing executing terminal context switching.

    // disable_irq(KB_IRQ);
    if(current_terminal_displaying == 0){
        memcpy(buffer, buf1, terminals[0].kb_buffer_position);
        clearBuffer(0);
        // printfBetter("displaying 0\n");
    }
    if(current_terminal_displaying == 1){
        memcpy(buffer, buf2, terminals[1].kb_buffer_position);
        clearBuffer(1);
        // printfBetter("displaying 1\n");
    }
    if(current_terminal_displaying == 2){
        memcpy(buffer, buf3, terminals[2].kb_buffer_position);
        clearBuffer(2);
        // printfBetter("displaying 2\n");
    }
    // printfBetter("\nbuf1:");
    // printfBetter(buf1);
    // printfBetter("\nbuf2:");
    // printfBetter(buf2);
    // printfBetter("\nbuf3:");
    // printfBetter(buf3);
    // printfBetter("\n");


    return terminals[current_terminal_executing].kb_buffer_position;
    // return 6;
}






/* keyboard_handler
 * 
 * Handles the keyboard interrupts. Only writes to a buffer if the current executing terminal
 * Is also the current displaying terminal
 * Inputs: None
 * Outputs: None
 * Side Effects: Handles the keyboard interrupts
 * Coverage: Keyboard interrupt handling
 * Files: None
 */
void keyboard_handler(){
    cli();
    // disable_irq(PIT_IRQ);
    uint8_t input = inb(KB_DATAPORT);
    disable_irq(KB_IRQ);
    if (input == CAPSLOCK_CODE){
        capsLock ^= 1; // flips the status of capsLock
    } else if ((input==LEFT_SHIFT_PRESSED_CODE) || (input==RIGHT_SHIFT_PRESSED_CODE)){
        shift = 1;
    } else if ((input==LEFT_SHIFT_RELEASED_CODE) || (input==RIGHT_SHIFT_RELEASED_CODE)){
        shift = 0;
    } else if (input==CTRL_PRESSED_CODE){
        ctrl = 1;
    } else if (input==CTRL_RELEASED_CODE){
        ctrl = 0;
    } else if (input==ALT_PRESSED_CODE){
        alt = 1;
    } else if (input==ALT_RELEASED_CODE){
        alt = 0;
    } else if (alt){
            if (input == F1_CODE){ // switch to terminal 0
                displaying_terminal_switch(0);
            } else if (input == F2_CODE){ // switch to terminal 1
                displaying_terminal_switch(1);
            } else if (input == F3_CODE){ // switch to terminal 2
                displaying_terminal_switch(2);
            }
    } else if (ctrl){
            if (input == L_CODE){ // ctrl + l = clear screen and reset cursor to top left
                clear();
                setCursor(0,0);
            }
    } else if (terminals[current_terminal_displaying].reading){
        uint32_t savedVramAddress = get_vram_address();
        change_vram_address(0xb8000);
        if (input == ENTER_CODE){
            // if (current_terminal_executing == 0){
            //     printfBetter("enter t0, buf: ");\
            // }
            // if (current_terminal_executing == 1){
            //     printfBetter("enter t1, buf: ");\
            // }
            // if (current_terminal_executing == 2){
            //     printfBetter("enter t2, buf: ");\
            // }
            terminals[current_terminal_displaying].reading = 0;
            addToBuffer(terminals[current_terminal_displaying].kb_buffer_position,'\0');
            // putcBetter('\n');
        } else if (input == BACKSPACE_CODE){
            if (terminals[current_terminal_displaying].kb_buffer_position > 0){
                addToBuffer(terminals[current_terminal_displaying].kb_buffer_position-1,' ');
                terminals[current_terminal_displaying].kb_buffer_position--;
                setCursor(getCursorX()-1,getCursorY());
                putcBetter(' ');
                setCursor(getCursorX()-1,getCursorY());
            }
        } else if ((terminals[current_terminal_displaying].kb_buffer_position<length) && (input<=0x3d)){ // x39 is the last index in the scan code arrays
            if (shift){
                addToBuffer(terminals[current_terminal_displaying].kb_buffer_position,scanTableShift[input]);// add character to buffer we're currently writing to
                putcBetter(scanTableShift[input]);
            }  else if (capsLock){
                addToBuffer(terminals[current_terminal_displaying].kb_buffer_position,scanTableCapsLock[input]);// add character to buffer we're currently writing to
                putcBetter(scanTableCapsLock[input]);
            } else {
                addToBuffer(terminals[current_terminal_displaying].kb_buffer_position,scanTable[input]);// add character to buffer we're currently writing to
                putcBetter(scanTable[input]);
            }
            terminals[current_terminal_displaying].kb_buffer_position++;
        }
        change_vram_address(savedVramAddress);
    }

    send_eoi(KB_IRQ);
    enable_irq(KB_IRQ);
    // enable_irq(PIT_IRQ);
    sti();
    
    asm volatile(" \n\
    leave \n\
    iret"
    );
}

