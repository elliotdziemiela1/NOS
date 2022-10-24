#include "../types.h"
#include "terminal.h"
#include "../lib.h"

char buf[BUFFER_SIZE]; 
int pos; // position in buffer to write next character (0 indexed)
int opened; // flag for whether or not the terminal is currently opened.

static void acceptNewCommand(){ // THIS CODE NEEDS TO BE CHANGED
    char path[10] = {'s','o','m','e','w','h','e','r','e',':'};
    int i;
    for (i = 0; i < PATH_LENGTH; i++){
        buf[i] = path[i];
    }
    for (i = PATH_LENGTH; i < BUFFER_SIZE-1; i++){
        buf[i] = ' ';
    }

    buf[BUFFER_SIZE-1] = '\n'; // signifies end of buffer

    pos = PATH_LENGTH;

    printfBetter(buf);
    setCursor(pos, getCursorY()-2);
}

uint32_t terminal_open(){
    clear();
    setCursor(0,0);
    opened = 1;
    return 0;
}

uint32_t terminal_write(uint8_t * in, int32_t nbytes){
    if (!opened || !in)
        return -1;
    int i=0;
    for (i = 0; i < nbytes; i++){
        putcBetter(*in);
        in++;
    }
    putcBetter('\n');

    return i;
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
    gets(buf,BUFFER_SIZE-PATH_LENGTH);
    
    setCursor(0, getCursorY()+1);
    if (getCursorY() >= NUM_ROWS){
        verticalScroll(1);
        setCursor(0,NUM_ROWS-1);
    }
    return pos;
}



