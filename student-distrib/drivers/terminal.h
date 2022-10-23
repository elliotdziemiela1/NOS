#include "../types.h"
#define BUFFER_SIZE 128
#define PATH_LENGTH 10 // length of path string displayed at left of terminal screen

uint32_t terminal_open();
uint32_t terminal_read();
uint32_t terminal_write(uint8_t * in, int32_t nbytes);
