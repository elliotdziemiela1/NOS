#include "x86_desc.h"
#include "types.h"
#include "lib.h"
#include "idt.h"

// ptr = address of handler function to insert into IDT.
void insert_handler(int irqNum, uint32_t ptr, int dpl){
    SET_IDT_ENTRY(idt[0x20 + irqNum], ptr, dpl); //x20 = start of irqs in idt
}

void dummyrtcHandler(){printf("rtc interrupt handler called");while(1){}}
void dummykbHandler(){printf("keyboard interrupt handler called");while(1){};}
void sysCallHandler(){printf("system call handler called");while(1){};}
void divZeroHandler(){printf("Exception: Divide Error");while(1){};}
void resHandler(){printf("Exception: RESERVED");while(1){};}
void nmiHandler(){printf("NMI Interrupt");while(1){};}
void bpHandler(){printf("Exception: Breakpoint");while(1){};}
void ofHandler(){printf("Exception: Overflow");while(1){};}
void breHandler(){printf("Exception: BOUND Range Exceeded");while(1){};}
void invopHandler(){printf("Exception: Invalid Opcode");while(1){};}
void dnaHandler(){printf("Exception: Device Not Available");while(1){};}
void dfHandler(){printf("Exception: Double Fault");while(1){};}
void csoHandler(){printf("Exception: Coprocessor Segment Overrun");while(1){};}
void itssHandler(){printf("Exception: Invalid TSS");while(1){};}
void snpHandler(){printf("Exception: Segment Not Present");while(1){};}
void ssfHandler(){printf("Exception: Stack-Segment Fault");while(1){};}
void gpHandler(){printf("Exception: General Protection");while(1){};}
void pfHandler(){printf("Exception: Page Fault");while(1){};}
void mfHandler(){printf("Exception: Math Fault");while(1){};}
void acHandler(){printf("Exception: Alignment Check");while(1){};}
void mcHandler(){printf("Exception: Machine Check");while(1){};}
void simdfpHandler(){printf("Exception: SIMD Floating-Point");while(1){};}

void init_idt(){
    // irq8 = rtc
    SET_IDT_ENTRY(idt[0x28], &dummyrtcHandler, 0);

    // irq1 = keyboard
    SET_IDT_ENTRY(idt[0x21], &dummykbHandler, 0);

    // system calls
    SET_IDT_ENTRY(idt[0x80], &sysCallHandler, 3);

    // divide by 0 exception
    SET_IDT_ENTRY(idt[0x0], &divZeroHandler, 0);

    // RESERVED exception
    SET_IDT_ENTRY(idt[0x1], &resHandler, 0);

    // NMI exception
    SET_IDT_ENTRY(idt[0x2], &nmiHandler, 0);

    // Breakpoint exception
    SET_IDT_ENTRY(idt[0x3], &bpHandler, 0);

    // Overflow exception
    SET_IDT_ENTRY(idt[0x4], &ofHandler, 0);

    // BOUND Range Exceeded exception
    SET_IDT_ENTRY(idt[0x5], &breHandler, 0);

    // Invalid Opcode exception
    SET_IDT_ENTRY(idt[0x6], &invopHandler, 0);

    // Device Not Available exception
    SET_IDT_ENTRY(idt[0x7], &dnaHandler, 0);

    // Double Fault exception
    SET_IDT_ENTRY(idt[0x8], &dfHandler, 0);

    // Coprocessor Segment Overrun exception
    SET_IDT_ENTRY(idt[0x9], &csoHandler, 0);

    // Invalid TSS exception
    SET_IDT_ENTRY(idt[0xa], &itssHandler, 0);

    // Segment Not Present exception
    SET_IDT_ENTRY(idt[0xb], &snpHandler, 0);

    // Stack-Segment Fault exception
    SET_IDT_ENTRY(idt[0xc], &ssfHandler, 0);

    // General Protection exception
    SET_IDT_ENTRY(idt[0xd], &gpHandler, 0);

    // Page Fault exception
    SET_IDT_ENTRY(idt[0xe], &pfHandler, 0);

    // Math Fault exception
    SET_IDT_ENTRY(idt[0x10], &mfHandler, 0);

    // Alignment Check exception
    SET_IDT_ENTRY(idt[0x11], &acHandler, 0);


    // Machine Check exception
    SET_IDT_ENTRY(idt[0x12], &mcHandler, 0);


    // SIMD Floating-Point exception
    SET_IDT_ENTRY(idt[0x13], &simdfpHandler, 0);
}

/* If want to make umbrella function, you can pull the exception number from the 
* top of the stack. The problem lies in finding the top of the stack at this point.*/
