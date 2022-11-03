#include "syscalls.h"
#include "./drivers/keyboard.h"

#define JUNK 0

int32_t halt (uint8_t status){
    return 0;
}
int32_t execute (const uint8_t* command){
    return 0;
}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    return 0;
}
int32_t open (const uint8_t* filename){
    return 0;
}
int32_t close (int32_t fd){
    return 0;
}

/* int32_t read (int32_t fd, void* buf, int32_t nbytes);
 * Inputs: - fd= file descriptor we want to call the read function of.
 *         - buf= buffer we want to fill with read bytes
 *         - nbytes= number of bytes to fill buf with
 * Return Value: returns # of bytes read
 * Function: if fd == 0, we will read from the keyboard by calling gets. Otherwise,
 *          we will call the read function given in the file descriptor fd.
 * */
// can refer to ece391hello.c for an example of a call to this function
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    if (fd != 0){
        // Should call linkage wrapper function here
        return ((file_desc_t *)fd)->fop_func.read(JUNK, buf, nbytes); 
    } else { // else read from keyboard which doesn't have a fd
        return gets(buf,nbytes);
    }
}

