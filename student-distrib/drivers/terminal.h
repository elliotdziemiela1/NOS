#pragma once
#include "../types.h"
#include "../scheduling.h"

#define BUFFER_SIZE 128 

typedef struct terminal {
    int8_t terminal_id;
    int32_t active_process_pid;

    int8_t process_flag;

    int32_t screen_x;
    int32_t screen_y;

    volatile uint8_t keyboard_buffer[BUFFER_SIZE];
    volatile uint8_t keyboard_buffer_idx;

    volatile int keyboard_buffer_enable;

    //pointer to this terminals video memory
    uint32_t *video_mem;

} terminal_t;

uint32_t terminal_open();
uint32_t terminal_close();
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf1, int32_t nbytes);

//Scheduling Functions
void initialize_terminals();

