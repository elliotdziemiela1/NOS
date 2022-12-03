#include "pit.h"
#include "../idt.h"
#include "../i8259.h"
#include "../lib.h"
#include "rtc.h"
#include "../scheduling.h"
#include "../syscalls.h"

volatile uint32_t pit_timer = 0;
uint8_t terminal_to_start = 0;

void pit_init(){
    insert_handler(PIT_IRQ, (int) &pit_handler, 0); //0 for dpl 0

    outb(PIT_MODE, PIT_REG_CMD);
    outb((PIT_INTERVAL / HZ) & 0xFF, PIT_REG_DATA);
    outb(((PIT_INTERVAL / HZ) >> 8), PIT_REG_DATA);

    enable_irq(PIT_IRQ);
}

void pit_handler(){
    cli();
    pit_timer++;
    if(terminal_to_start == 0){
        current_terminal_executing = terminal_to_start;
        displaying_terminal_switch(current_terminal_executing);
        sti();
        send_eoi(PIT_IRQ);
        terminal_to_start ++;
        clear();
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
                 
        sti();
        send_eoi(PIT_IRQ);
        terminal_to_start ++;
        execute((const uint8_t *)"shell");
    }

    if(terminal_to_start == 3){
        displaying_terminal_switch(0);
        terminal_to_start++;
    }

    // if(pit_timer % 10 == 0){
	// 		printfBetter("counter val: %d", pit_timer);
	// }

    sti();
    send_eoi(PIT_IRQ);
    schedule_context_switch();
    
    asm volatile(" \n\
    leave \n\
    iret"
    );
    //Context switch into next terminal
}
