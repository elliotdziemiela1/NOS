#include "../types.h"
#include "terminal.h"
#include "../lib.h"
#include "keyboard.h"

char buf[BUFFER_SIZE]; 
int pos; // position in buffer to write next character (0 indexed)
int opened; // flag for whether or not the terminal is currently opened.

/* acceptNewCommand()
 * Inputs: none
 * Outputs: 
 * Return Value: none
 * Function: moves cursor down and prints shell path to screen, then sets cursor 
 *           after shell path to accept input from user
 */
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

/* terminal_open
 * Inputs: none
 * Outputs: 
 * Return Value: none
 * Function: moves cursor down and prints shell path to screen, then sets cursor 
 *           after shell path to accept input from user
 */
uint32_t terminal_open(){
    clear();
    setCursor(0,0);
    opened = 1;
    return 0;
}

/* uint32_t terminal_write(char * in, int32_t nbytes)
 * Inputs: - in= pointer to string to write to the screen
 *         - nbytes= number of bytes or characters in string
 * Return Value: returns the number of bytes/chars written to the screen
 * Function: writes the characters from input array "in" into the screen
 * */
uint32_t terminal_write(char * in, int32_t nbytes){
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

/* uint32_t terminal_read()
 * Inputs: none
 * Return Value: returns the number of bytes/chars read to terminal buffer
 * Function: reads a string from the keyboard and writes to the terminal buffer
 *           as well as printing to the screen in gets.
 * */
uint32_t terminal_read(){
    acceptNewCommand();
    if (!opened) // if we have not yet opened the terminal, do nothing
        return -1;
    uint32_t out = gets(buf,BUFFER_SIZE-PATH_LENGTH); // retrieve keyboard input
    
    setCursor(0, getCursorY()+1); 
    if (getCursorY() >= NUM_ROWS){
        verticalScroll(1);
        setCursor(0,NUM_ROWS-1);
    }
    return out;
}



