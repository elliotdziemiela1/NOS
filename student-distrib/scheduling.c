#include "scheduling.h"
#include "syscalls.h"
#include "paging.h"
#include "x86_desc.h"
#include "i8259.h"
#include "drivers/keyboard.h"
#include "./drivers/pit.h"
#include "lib.h"

void schedule_context_switch(){
    uint8_t old_pid = terminals[current_terminal_executing].active_process_pid;
    pcb_t * old_pcb = get_pcb(old_pid);

    // update current terminal executing
    current_terminal_executing = (current_terminal_executing + 1) % 3;
    // while (terminals[(current_terminal_executing + 1) % 3].active_process_pid == -1) // round robin
    //     current_terminal_executing = (current_terminal_executing + 1) % 3;

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
    tss.esp0 = EIGHT_MB - EIGHT_KB * (new_pid) - 4;

    // change virtual program page mapping to next program
    uint32_t physical_address = (2 + new_pid) * FOURMB; //2:you want to skip the first page which houses the kernel
    page_dir[MB_128_PAGE].fourmb.addr = physical_address / FOURKB;

    //printf("IIIIIIIIIIIIIIIIIII \n");
    //setCursor(terminals[current_terminal_executing].screen_x, terminals[current_terminal_executing].screen_y);

    /* Perform context switch, swap ESP/EBP with that of the registers */
    asm volatile(
                 "movl %%esp, %%eax;"
                 "movl %%ebp, %%ebx;"
                 :"=a"(old_pcb->saved_process_esp), "=b"(old_pcb->saved_process_ebp)    /* outputs */
                 :                                          /* no input */
                 );
    
    asm volatile(
                 "movl %%eax, %%esp;"
                 "movl %%ebx, %%ebp;"
                 "leave;"
                 "ret;"
                 :                                          /* no outputs */
                 :"a"(new_pcb->saved_process_esp), "b"(new_pcb->saved_process_ebp)    /* input */
                 );

    // send_eoi(PIT_IRQ);
    // enable_irq(PIT_IRQ); 
    // asm volatile(
    //     "leave;"
    //     "ret;"
    // );
    return;
}

uint8_t displaying_terminal_switch(uint8_t newTerminalNum){ // 0,1, or 2
    switch_vram(current_terminal_displaying, newTerminalNum);
    terminals[current_terminal_displaying].screen_x = getCursorX(); // saves cursor x
    terminals[current_terminal_displaying].screen_y = getCursorY(); // saves cursor y
    setCursor(terminals[newTerminalNum].screen_x, terminals[newTerminalNum].screen_y);

    // 
    // CURRENTLY ONLY EXECUTES THE DISPLAYING TERMINAL
    // current_terminal_executing = newTerminalNum; // UNCOMMENT TO SHOW TYPING IN OTHER TERMINALS
    // 

    current_terminal_displaying = newTerminalNum;

    return 0;
}