#pragma once
#include "../types.h"
#include "../scheduling.h"
#include "../lib.h"


#define BUFFER_SIZE 128 
#define TOTAL_TERMINALS 3

typedef struct terminal_t {
    int8_t active_process_pid; // pid of the current running program in this terminal
    int8_t parent_process_pid; // pid of the program that called the current running program in this terminal

    int8_t process_flag; // 1 if a program is currently running on a terminal, 0 when terminal not set to shell
    int32_t rtc_mod; // factor to divide the rtc counter by (virtualizing rtc)

    int32_t screen_x; // x position of cursor on terminal screen
    int32_t screen_y; // y position of cursor on terminal screen

    uint32_t kb_buffer_position; // current position in keyboard buffer for the terminal
    uint8_t reading; // flag for keyboard handler to reference that says if this terminal is taking in a string from the keyboard right now

    char keyboard_buffer[BUFFER_SIZE];

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

