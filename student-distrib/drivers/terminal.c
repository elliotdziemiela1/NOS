#include "../types.h"
#include "terminal.h"
#include "../lib.h"
#include "keyboard.h"

static volatile char buf[BUFFER_SIZE]; 
static int pos; // position in buffer to write next character (0 indexed)
// static int opened; // flag for whether or not the terminal is currently opened.

/* acceptNewCommand()
 * Inputs: none
 * Outputs: 
 * Return Value: none
 * Function: moves cursor down and prints shell path to screen, then sets cursor 
 *           after shell path to accept input from user
 */
static void acceptNewCommand(){ // THIS CODE NEEDS TO BE CHANGED
    int i;
    for (i = 0; i < BUFFER_SIZE-1; i++){
        buf[i] = ' ';
    }
    buf[BUFFER_SIZE-1] = '\n'; // signifies end of buffer
    pos = 0;
}

/* terminal_open
 * Inputs: none
 * Outputs: 
 * Return Value: none
 * Function: moves cursor down and prints shell path to screen, then sets cursor 
 *           after shell path to accept input from user
 */
uint32_t terminal_open(){
    clear();
    // opened = 1;
    return -1;
}

uint32_t terminal_close(){
    clear();
    // opened = 0;
    return -1;
}

/* uint32_t terminal_write()
 * Inputs: none
 * Return Value: returns the number of bytes/chars written to the screen
 * Function: writes the characters from buffer to the screen
 * */
int32_t terminal_write(int32_t fd, const void* buf1, int32_t nbytes){
    int ret = printfBetter((int8_t *) buf1);
    return ret;
}

/* uint32_t terminal_read()
 * Inputs: none
 * Return Value: returns the number of bytes/chars read to terminal buffer
 * Function: reads a string from the keyboard and writes to the terminal buffer
 *           as well as printing to the screen in gets.
 * */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    acceptNewCommand();
    int32_t bytesRead = gets(buf,BUFFER_SIZE-1);
    putcBetter('\n');
    return bytesRead;
}



