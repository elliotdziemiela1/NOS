#pragma once

#include "lib.h"

#define KB_BUFFER_SIZE  128
#define TOTAL_TERMINALS 3

typedef struct terminal {
    int8_t terminal_id;
    int32_t active_process_pid;

    int8_t process_flag;

    int screen_x;
    int screen_y;

    volatile uint8_t keyboard_buffer[KB_BUFFER_SIZE];
    volatile uint8_t keyboard_buffer_idx;

    volatile int keyboard_buffer_enable;

    uint32_t *video_mem;

} terminal_t;

extern volatile terminal_t terminals[TOTAL_TERMINALS];
extern volatile int8_t current_terminal;

int8_t get_next_scheduled_process();

void schedule_context_switch(int8_t process_number);

