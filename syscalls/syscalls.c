#include <stdint.h>
#include "../student-distrib/drivers/keyboard.h"


/* int32_t __ece391_read (int32_t fd, void* buf, int32_t nbytes);
 * Inputs: - fd= file descriptor we want to call the read function of.
 *         - buf= buffer we want to fill with read bytes
 *         - nbytes= number of bytes to fill buf with
 * Return Value: returns # of bytes read
 * Function: if fd == 0, we will read from the keyboard by calling gets. Otherwise,
 *          we will call the read function given in the file descriptor fd.
 * */
// can refer to ece391hello.c for an example of a call to this function
int32_t ece391_read (int32_t fd, void* buf, int32_t nbytes){
    if (fd != 0){
        return ((file_descriptor_t *)fd)->read(buf, nbytes); // PLACEHOLDER UNTIL WE MAKE FDs
    } else { // else read from keyboard which doesn't have a fd
        return gets(buf,nbytes);
    }
}

int32_t ece391_halt (uint8_t status);
int32_t ece391_execute (const uint8_t* command);
int32_t ece391_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t ece391_open (const uint8_t* filename);
int32_t ece391_close (int32_t fd);
int32_t ece391_getargs (uint8_t* buf, int32_t nbytes);
int32_t ece391_vidmap (uint8_t** screen_start);
int32_t ece391_set_handler (int32_t signum, void* handler);
int32_t ece391_sigreturn (void);