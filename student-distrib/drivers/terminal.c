// terminal_fd = ece391_open((uint8_t*)"rtc");
// ece391_read(rtc_fd, &garbage, 4);

// int32_t ece391_open (const uint8_t* filename)
// {
//     uint32_t rval;

//     if (0 == ece391_strcmp (filename, (uint8_t*)".")) {
// 	dir = opendir (".");
//         dir_fd = open ("/dev/null", O_RDONLY);
// 	return dir_fd;
//     }

//     asm volatile ("INT $0x80" : "=a" (rval) :
// 		  "a" (5), "b" (filename), "c" (O_RDONLY));
//     if (rval > 0xFFFFC000)
//         return -1;
//     return rval;
// }

// int32_t 
// ece391_write (int32_t fd, const void* buf, int32_t nbytes)
// {
//     if (NULL == dir || dir_fd != fd)
//         return __ece391_write (fd, buf, nbytes);
//     return -1;
// }

#include "terminal.h"
#include "lib.h"
#define BUFFER_SIZE 128
#define DEL 0x7f // ascii code for delete char
#define PATH_LENGTH 10 // length of path string displayed at left of terminal screen

char buf[BUFFER_SIZE]; 
int length; // length of current string in buffer
char path[]; // string of current path to display left of command

static void clearBuf(){
    buf = { '_' }; // fills buffer with underscores
    buf[BUFFER_SIZE-1] = '\n'; // signifies end of buffer

    path = "somewhere:"
    int i;
    for (i = 0; i < PATH_LENGTH; i++){
        buf[i] = path[i];
    }

    length = 0;
}

uint32_t open(){
    clearBuf();
}


/* uint32_t write(void);
 * Inputs: - in= pointer to string to write to the terminal buffer
 *         - nbytes= length of string
 * Return Value: 0 if sensical character (non empty delete, non full write), otherwise -1
 * Function: I made it such that if you want to type character by character you would call write() 
 *          with a 1 character input string. If you want to maybe copy/paste into the terminal then  
 *          you can pass a whole string.
 * */
uint32_t write(uint8_t * in, int32_t nbytes){
    for (i = PATH_LENGTH; i < nbytes; i++){
        c = in[i];
        if (length < BUFFER_SIZE-2){ // buf[BUFFER_SIZE-1]=newline. buf[BUFFER_SIZE-2]=last char in buf
            if (c == DEL){
                if (length > 0){
                    buf[length-1] = '_';
                    length--;
                    return 0; // deleted char from non-empty buffer
                }
                return -1; // tried to delete when buffer was empty
            }
            buf[length] = c;
            length++;
            return 0; // added character to non-full buffer
        }
        return -1; // buffer is full
    }
    puts(buf); // FILLER - gotta figure out how to display this to static vram array in lib.c without
    // printing every character
}


