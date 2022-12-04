#include "pit.h"
#include "../idt.h"
#include "../i8259.h"
#include "../lib.h"
#include "rtc.h"
#include "../scheduling.h"
#include "../syscalls.h"
#include "../paging.h"

volatile uint32_t pit_timer = 0;
uint8_t terminal_to_start;

/* void pit_init;
 * Inputs: - None
 * Return Value: None
 * Function: Initializes the pit by inserting its handler in the IDT table and setting its frequency
 * */
void pit_init(){
    terminal_to_start = 0; // need to initialize terminal 0 to shell
    insert_handler(PIT_IRQ, (int) &pit_handler, 0); //0 for dpl 0

    outb(PIT_MODE, PIT_REG_CMD);
    outb((PIT_INTERVAL / HZ) & 0xFF, PIT_REG_DATA);
    outb(((PIT_INTERVAL / HZ) >> 8) & 0xFF, PIT_REG_DATA);

    enable_irq(PIT_IRQ);
}

/* void pit_handler;
 * Inputs: - None
 * Return Value: None
 * Function: Handler which is called whenever a PIT interrupt occurs. Manages context switches for first three shell execution and any subsequent processes
 * */
void pit_handler(){

    cli();
    disable_irq(PIT_IRQ);
    
    // set terminal 0 to shell
    if(terminal_to_start == 0){
        // switching screen to terminal 0
        current_terminal_executing = terminal_to_start;
        displaying_terminal_switch(current_terminal_executing);
        terminal_to_start ++;
        clear();
        
        // saving the stack segment
        tss.ss0 = KERNEL_DS;
        tss.esp0 = EIGHT_MB - EIGHT_KB * (0) - 4; // -4 is the offset to get the end of the previous pcb

        // change virtual program page mapping to next program
        uint32_t physical_address = (2 + 0) * FOURMB; //2:you want to skip the first page which houses the kernel
        page_dir[MB_128_PAGE].fourmb.addr = physical_address / FOURKB;

        //flush tlb; takes place after change in paging structure
        asm volatile("\
        mov %cr3, %eax    ;\
        mov %eax, %cr3    ;\
        ");

        // enable interrupts and send eoi
        sti();
        enable_irq(PIT_IRQ);
        send_eoi(PIT_IRQ);
        execute((const uint8_t *)"shell");
    }   

    // set terminal 1 and 2 to shell
    else if(terminal_to_start < 3){
        // switching screen to terminal 1/2
        current_terminal_executing = terminal_to_start;
        displaying_terminal_switch(current_terminal_executing);

        // getting previous active process
        int8_t term_pid = terminals[current_terminal_executing - 1].active_process_pid;
        pcb_t * term_pcb = (pcb_t *)get_pcb(term_pid);

        // saving previous terminal (0/1) esp and ebp
        asm volatile(
                 "movl %%esp, %%eax;"
                 "movl %%ebp, %%ebx;"
                 :"=a"(term_pcb->saved_process_esp), "=b"(term_pcb->saved_process_ebp)    /* outputs */
                 :                                          /* no input */
                 );
                 
        // saving the stack segment
        tss.ss0 = KERNEL_DS;
        tss.esp0 = EIGHT_MB - EIGHT_KB * (terminal_to_start) - 4; // -4 is the offset to get the end of the previous pcb

        // change virtual program page mapping to next program
        uint32_t physical_address = (2 + terminal_to_start) * FOURMB; //2:you want to skip the first page which houses the kernel
        page_dir[MB_128_PAGE].fourmb.addr = physical_address / FOURKB;

        //flush tlb; takes place after change in paging structure
        asm volatile("\
        mov %cr3, %eax    ;\
        mov %eax, %cr3    ;\
        ");

        terminal_to_start ++;

        sti();
        enable_irq(PIT_IRQ);
        send_eoi(PIT_IRQ);
        
        execute((const uint8_t *)"shell");
    }
    // disable_irq(PIT_IRQ);

    if(terminal_to_start == 3){
        displaying_terminal_switch(0);
        terminal_to_start++;
    }

    // enable interrupts and send eoi
    sti();
    enable_irq(PIT_IRQ);
    send_eoi(PIT_IRQ);
    schedule_context_switch();
    
    asm volatile(" \n\
    leave \n\
    iret"
    );
}
