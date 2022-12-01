#include "scheduling.h"
#include "syscalls.h"

volatile terminal_t terminals[TOTAL_TERMINALS];
volatile int8_t current_terminal;

void schedule_context_switch(){
    uint8_t old_pid = terminals[current_terminal].active_process_pid;
    pcb_t * old_pcb = get_pcb(old_pid);

    uint8_t new_pid = get_next_scheduled_pid();
    pcb_t * new_pcb = get_pcb(new_pid);

    
}


int8_t get_next_scheduled_pid(){
    int i;

    int8_t next_terminal = (current_terminal + 1) % 3;

    return (int8_t) terminals[next_terminal].active_process_pid;
    
}

uint8_t displaying_terminal_switch(uint8_t newTerminalIdx){
    
    current_displaying_terminal = newTerminalIdx;
    return 0;
}
