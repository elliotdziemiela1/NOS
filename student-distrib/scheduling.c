#include "scheduling.h"
#include "syscalls.h"
#include "paging.h"
#include "x86_desc.h"

void schedule_context_switch(){
    uint8_t old_pid = terminals[current_terminal_executing].active_process_pid;
    pcb_t * old_pcb = get_pcb(old_pid);

    // update current terminal executing
    current_terminal_executing = ((current_terminal_executing + 1) % 3) + 1; // +1 since 1-indexed
    uint8_t new_pid = terminals[current_terminal_executing].active_process_pid;
    pcb_t * new_pcb = get_pcb(new_pid);
    
    // change vram pointer in lib.c
    if (current_terminal_executing == current_terminal_displaying){
        change_vram_address(VIDEO);
    } else {
        change_vram_address(terminals[current_terminal_executing].video_mem);
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

uint8_t displaying_terminal_switch(uint8_t newTerminalNum){
    switch_vram(current_terminal_displaying, newTerminalNum);
    terminals[current_terminal_displaying].screen_x = getCursorX(); // saves cursor x
    terminals[current_terminal_displaying].screen_y = getCursorY(); // saves cursor y
    setCursor(terminals[newTerminalNum].screen_x, terminals[newTerminalNum].screen_y);

    // 
    // CURRENTLY ONLY EXECUTES THE DISPLAYING TERMINAL
    current_terminal_executing = newTerminalNum; // PLACEHOLDER UNTIL WE CALL CONTEXT SWITCH
    // 

    current_terminal_displaying = newTerminalNum;
    return 0;
}