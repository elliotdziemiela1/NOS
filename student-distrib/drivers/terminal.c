#include "../types.h"
#include "terminal.h"
#include "../lib.h"
#include "keyboard.h"

char buf[BUFFER_SIZE]; 
int pos; // position in buffer to write next character (0 indexed)
int opened; // flag for whether or not the terminal is currently opened.

static void acceptNewCommand(){ // THIS CODE NEEDS TO BE CHANGED
    char path[11] = {'s','o','m','e','w','h','e','r','e',':','\0'};
    int i;
    for (i = 0; i < BUFFER_SIZE-1; i++){
        buf[i] = ' ';
    }

    buf[BUFFER_SIZE-1] = '\n'; // signifies end of buffer

    pos = 0;

    printfBetter(path);
    // printfBetter(buf);
    // setCursor(pos, getCursorY()-2);
}

uint32_t terminal_open(){
    clear();
    opened = 1;
    return 0;
}

uint32_t terminal_close(){
    clear();
    opened = 0;
    return 0;
}

uint32_t terminal_write(){
    if (!opened)
        return -1;
    // char * ptr = buf;
    // while (ptr != (buf+BUFFER_SIZE)){
    //     putcBetter(*ptr);
    //     ptr++;
    // }
    int ret = printfBetter(buf);
    return ret;
}


/* uint32_t write(void);
 * Inputs: - in= pointer to string to write to the terminal buffer
 *         - nbytes= number of bytes or characters in string
 * Return Value: returns the number of bytes/chars written to the terminal buffer
 * Function: writes the characters from input array "in" into the terminal buffer.
 * */
uint32_t terminal_read(){
    acceptNewCommand();
    if (!opened)
        return -1;
    int32_t bytesRead = gets(buf,BUFFER_SIZE-1);

    return bytesRead;
}



