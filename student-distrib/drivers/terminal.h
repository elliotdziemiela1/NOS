#include "../types.h"
#define BUFFER_SIZE 128 

uint32_t terminal_open();
uint32_t terminal_close();
uint32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
uint32_t terminal_write(int32_t fd, const void* buf1, int32_t nbytes);
