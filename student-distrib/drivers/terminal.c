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
    // char path[11] = {'s','o','m','e','w','h','e','r','e',':','\0'};
    int i;
    for (i = 0; i < BUFFER_SIZE-1; i++){
        buf[i] = ' ';
    }
    buf[BUFFER_SIZE-1] = '\n'; // signifies end of buffer
    pos = 0;

    // printfBetter(path);
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
    // if (!opened)
    //     return -1;
    // char * ptr = buf;
    // while (ptr != (buf+BUFFER_SIZE)){
    //     putcBetter(*ptr);
    //     ptr++;
    // }
    int ret = printfBetter((int8_t *) buf1);
    // if (ret)
    //     putcBetter('\n');
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
    // if (!opened) // if we have not yet opened the terminal, do nothing
    //     return -1;
    int32_t bytesRead = gets(buf,BUFFER_SIZE-1);
    putcBetter('\n');
    // printf("ali test 1");
    return bytesRead;
}



