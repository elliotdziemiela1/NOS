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
    terminal_to_start = 0;
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
    // send_eoi(PIT_IRQ);
    cli();
    disable_irq(PIT_IRQ);
    // pit_timer++;
    // if(pit_timer < 15){
    //     printfBetter("cte: %d test: %d \n", current_terminal_executing, pit_timer);
    // }
    if(terminal_to_start == 0){
        current_terminal_executing = terminal_to_start;
        displaying_terminal_switch(current_terminal_executing);
        terminal_to_start ++;
        clear();
        
        tss.ss0 = KERNEL_DS;
        tss.esp0 = EIGHT_MB - EIGHT_KB * (0) - 4;

        // change virtual program page mapping to next program
        uint32_t physical_address = (2 + 0) * FOURMB; //2:you want to skip the first page which houses the kernel
        page_dir[MB_128_PAGE].fourmb.addr = physical_address / FOURKB;
        //flush tlb; takes place after change in paging structure
        asm volatile("\
        mov %cr3, %eax    ;\
        mov %eax, %cr3    ;\
        ");

        sti();
        enable_irq(PIT_IRQ);
        send_eoi(PIT_IRQ);
        execute((const uint8_t *)"shell");
    }else if(terminal_to_start < 3){
        current_terminal_executing = terminal_to_start;
        displaying_terminal_switch(current_terminal_executing);

        int8_t term_pid = terminals[current_terminal_executing - 1].active_process_pid;
        pcb_t * term_pcb = get_pcb(term_pid);

        asm volatile(
                 "movl %%esp, %%eax;"
                 "movl %%ebp, %%ebx;"
                 :"=a"(term_pcb->saved_process_esp), "=b"(term_pcb->saved_process_ebp)    /* outputs */
                 :                                          /* no input */
                 );
                 

        tss.ss0 = KERNEL_DS;
        tss.esp0 = EIGHT_MB - EIGHT_KB * (terminal_to_start) - 4;

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
        // printf("pre terminal display switch");
        
        displaying_terminal_switch(0);
        terminal_to_start++;
        // printf("post terminal display switch");
    }

    // if(pit_timer % 10 == 0){
	// 		printfBetter("counter val: %d", pit_timer);
	// }
    // if(current_terminal_executing == 0){
    //     printf("XXXXXXXXXXXXXXXXXXX \n");
    // }
    sti();
    enable_irq(PIT_IRQ);
    send_eoi(PIT_IRQ);
    schedule_context_switch();
    //enable_irq(PIT_IRQ);
    
    asm volatile(" \n\
    leave \n\
    iret"
    );
}
