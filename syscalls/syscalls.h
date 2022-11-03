#include <stdint.h>

int32_t ece391_halt (uint8_t status);
int32_t ece391_execute (const uint8_t* command);
int32_t ece391_read (int32_t fd, void* buf, int32_t nbytes);
int32_t ece391_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t ece391_open (const uint8_t* filename);
int32_t ece391_close (int32_t fd);
int32_t ece391_getargs (uint8_t* buf, int32_t nbytes);
int32_t ece391_vidmap (uint8_t** screen_start);
int32_t ece391_set_handler (int32_t signum, void* handler);
int32_t ece391_sigreturn (void);