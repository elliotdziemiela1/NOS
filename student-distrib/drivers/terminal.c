#include "../types.h"
#include "terminal.h"
#include "../lib.h"
#define BUFFER_SIZE 128
#define PATH_LENGTH 10 // length of path string displayed at left of terminal screen

char buf[BUFFER_SIZE]; 
int pos; // position in buffer to write next character (0 indexed)

static void acceptNewCommand(){ // THIS CODE NEEDS TO BE CHANGED
    char path[10] = {'s','o','m','e','w','h','e','r','e',':'};
    int i;
    for (i = 0; i < PATH_LENGTH; i++){
        buf[i] = path[i];
    }
    for (i = PATH_LENGTH; i < BUFFER_SIZE-1; i++){
        buf[i] = '_';
    }

    buf[BUFFER_SIZE-1] = '\n'; // signifies end of buffer

    pos = PATH_LENGTH;

    printf(buf);
    setCursorX(pos);
}

uint32_t terminal_open(){
    clear();
    acceptNewCommand();
    return 0;
}

uint32_t terminal_read(){
    return 0;
}


/* uint32_t write(void);
 * Inputs: - in= pointer to string to write to the terminal buffer
 *         - nbytes= number of bytes or characters in string
 * Return Value: returns the number of bytes/chars written to the terminal buffer
 * Function: writes the characters from input array "in" into the terminal buffer.
 * */
uint32_t terminal_write(uint8_t * in, int32_t nbytes){
    int i;
    for (i = 0; i < nbytes; i++){
        char c = in[i];
        if (pos < BUFFER_SIZE-2){ // buf[BUFFER_SIZE-1]=newline. buf[BUFFER_SIZE-2]=last char in buf
            buf[pos] = c;
            pos++;
        }
    }
    acceptNewCommand();
    return pos;
}



