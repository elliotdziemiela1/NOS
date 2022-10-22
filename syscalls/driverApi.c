#include <stdint.h>


/* int32_t __ece391_read (int32_t fd, void* buf, int32_t nbytes);
 * Inputs: - fd= file descriptor we want to call the read function of.
 *         - buf= buffer we want to fill with read bytes
 *         - nbytes= number of bytes to fill buf with
 * Return Value: 
 * Function: 
 * */
// can refer to ece391hello.c for an example of a call to this function
int32_t __ece391_read (int32_t fd, void* buf, int32_t nbytes){
    if (fd != 0){
        return ((file_descriptor_t *)fd)->read(buf, nbytes); // PLACEHOLDER UNTIL WE MAKE FDs
    } else { // else read from keyboard which doesn't have a fd
        
    }
}
int32_t __ece391_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t __ece391_close (int32_t fd);