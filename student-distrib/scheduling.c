#include "scheduling.h"
#include "syscalls.h"
#include "paging.h"
#include "x86_desc.h"
#include "i8259.h"
#include "drivers/keyboard.h"
#include "./drivers/pit.h"
#include "lib.h"


/* void schedule_context_switch;
 * Inputs: - None
 * Return Value: None
 * Function: Handles the context switching. Only called when there are three shells running in each terminal.
 * */
void schedule_context_switch(){
    //get currently executing process
    uint8_t old_pid = terminals[current_terminal_executing].active_process_pid;
    pcb_t * old_pcb = get_pcb(old_pid);

    // update current terminal executing
    current_terminal_executing = (current_terminal_executing + 1) % TOTAL_TERMINALS;

    //get process to execute next
    uint8_t new_pid = terminals[current_terminal_executing].active_process_pid;
    pcb_t * new_pcb = get_pcb(new_pid);
    
    // change vram pointer in lib.c
    if (current_terminal_executing == current_terminal_displaying){
        change_vram_address(VIDEO);
        vidremap(VIDEO);
    } else {
        change_vram_address(terminals[current_terminal_executing].video_mem);
        vidremap((uint32_t)(terminals[current_terminal_executing].video_mem));
    }

    //saving the tss
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - EIGHT_KB * (new_pid) - 4; // -4 is the offset to get the end of the previous pcb

    // change virtual program page mapping to next program
    uint32_t physical_address = (2 + new_pid) * FOURMB; //2:you want to skip the first page which houses the kernel
    page_dir[MB_128_PAGE].fourmb.addr = physical_address / FOURKB;

    //flush tlb; takes place after change in paging structure
    asm volatile("\
    mov %cr3, %eax    ;\
    mov %eax, %cr3    ;\
    ");


    /* Perform context switch, swap and save ESP/EBP with that of the registers */
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

    
    return;
}

/* uint8_t displaying terminal_switch;
 * Inputs: - uint8_t newTerminalNum - the index of the terminal that we want to switch into
 * Return Value: None
 * Function: Changes the video memory mapping to switch to the terminal that we want to switch to. Also saves the VRAM of the current_displaying_terminal to its respective video page
 * */
uint8_t displaying_terminal_switch(uint8_t newTerminalNum){ // 0,1, or 2
    if(newTerminalNum < 0 || newTerminalNum > 2){ //invalid terminal number
        return -1;
    }

    // save current vram page, restore next vram page
    switch_vram(current_terminal_displaying, newTerminalNum);

    // update displaying terminal number to next terminal 
    current_terminal_displaying = newTerminalNum;
    return 0;
}