#include "scheduling.h"
#include "syscalls.h"
#include "paging.h"
#include "syscalls.h"
#include "x86_desc.h"

volatile terminal_t current_terminal_executing;
volatile terminal_t current_terminal_displaying;



void schedule_context_switch(){
    uint8_t old_pid = terminals[current_terminal_executing.terminal_id].active_process_pid;
    pcb_t * old_pcb = get_pcb(old_pid);

    uint8_t new_pid = get_next_scheduled_pid();
    pcb_t * new_pcb = get_pcb(new_pid);

    switch_vram(old_pid, new_pid);
    
    if(current_terminal_executing.terminal_id != current_terminal_displaying.terminal_id){
        remap_VRAM((uint32_t) current_terminal_executing.video_mem);
    }

    //saving the tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - EIGHT_KB * (new_pid + 1) - 4;

    /* Perform context switch, swap ESP/EBP with that of the registers */
    asm volatile(
                 "movl %%esp, %%eax;"
                 "movl %%ebp, %%ebx;"
                 :"=a"(old_pcb->saved_esp), "=b"(old_pcb->saved_ebp)    /* outputs */
                 :                                          /* no input */
                 );
                 
    asm volatile(
                 "movl %%eax, %%esp;"
                 "movl %%ebx, %%ebp;"
                 :                                          /* no outputs */
                 :"a"(new_pcb->saved_esp), "b"(new_pcb->saved_ebp)    /* input */
                 );
    return;
}


int8_t get_next_scheduled_pid(){
    int i;

    int8_t next_terminal = (current_terminal_executing.terminal_id + 1) % 3;

    return (int8_t) terminals[next_terminal].active_process_pid;
    
}

uint8_t displaying_terminal_switch(uint8_t newTerminalIdx){
    
    current_displaying_terminal = newTerminalIdx;
    return 0;
}
