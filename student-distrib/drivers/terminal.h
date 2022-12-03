#pragma once
#include "../types.h"
#include "../scheduling.h"
#include "../lib.h"


#define BUFFER_SIZE 128 
#define TOTAL_TERMINALS

typedef struct terminal_t {
    int32_t active_process_pid;

    int8_t process_flag;

    int32_t screen_x;
    int32_t screen_y;

    uint32_t kb_buffer_position;
    uint8_t reading; // flag for keyboard handler to reference that says if this terminal is taking in
    // a string from the keyboard right now

    // volatile uint8_t keyboard_buffer[BUFFER_SIZE];
    // volatile uint8_t keyboard_buffer_idx;
    // volatile int keyboard_buffer_enable;

    //pointer to this terminals video memory
    uint32_t *video_mem; // 32 bit address. Either 0xb8000, 0xb9000, or 0xba000

} terminal_t;

volatile terminal_t terminals[TOTAL_TERMINALS];

volatile uint8_t current_terminal_executing; // 0 indexed: 0,1, or 2
volatile uint8_t current_terminal_displaying; // 0 indexed: 0,1, or 2

uint32_t terminal_open();
uint32_t terminal_close();
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf1, int32_t nbytes);

//Scheduling Functions
void initialize_terminals();

