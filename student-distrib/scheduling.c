#include "scheduling.h"

volatile terminal_t terminals[TOTAL_TERMINALS];
volatile int8_t current_terminal;

int8_t get_next_scheduled_process(){
    int i;

    int8_t next_terminal = current_terminal;
   
    for(i = 0; i < TOTAL_TERMINALS; i++){
        
        next_terminal = (current_terminal + i) % 3;
        if(terminals[next_terminal].process_flag){
            break;
        }
    }

    return (int8_t) terminals[next_terminal].active_process_pid;
    
}

