#include "x86_desc.h"
#include "types.h"
#define KERNEL_CODE_SEGMENT 0x0

void rtcHandler(){printf("rtc interrupt handler called");while(1){}}
void kbHandler(){printf("keyboard interrupt handler called");while(1){};}
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
    gate_desc_t irq8;
    irq8.seg_selector = KERNEL_CODE_SEGMENT;
    irq8.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(irq8, &rtcHandler);
    idt[0x21] = irq8;

    // irq1 = keyboard
    gate_desc_t irq1;
    irq1.seg_selector = KERNEL_CODE_SEGMENT;
    irq1.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(irq1, &kbHandler);
    idt[0x21] = irq1;

    // system calls
    gate_desc_t sysCall;
    sysCall.seg_selector = KERNEL_CODE_SEGMENT;
    sysCall.dpl = 3; //privilege level =0 (user)
    SET_IDT_ENTRY(sysCall, &sysCallHandler);
    idt[0x80] = sysCall;

    // divide by 0 exception
    gate_desc_t zero;
    zero.seg_selector = KERNEL_CODE_SEGMENT;
    zero.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(zero, &divZeroHandler);
    idt[0x0] = zero;

    // RESERVED exception
    gate_desc_t res;
    res.seg_selector = KERNEL_CODE_SEGMENT;
    res.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(res, &resHandler);
    idt[0x1] = res;

    // NMI exception
    gate_desc_t nmi;
    nmi.seg_selector = KERNEL_CODE_SEGMENT;
    nmi.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(nmi, &nmiHandler);
    idt[0x2] = nmi;

    // Breakpoint exception
    gate_desc_t bp;
    bp.seg_selector = KERNEL_CODE_SEGMENT;
    bp.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(bp, &bpHandler);
    idt[0x3] = bp;

    // Overflow exception
    gate_desc_t of;
    of.seg_selector = KERNEL_CODE_SEGMENT;
    of.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(of, &ofHandler);
    idt[0x4] = of;

    // BOUND Range Exceeded exception
    gate_desc_t bre;
    bre.seg_selector = KERNEL_CODE_SEGMENT;
    bre.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(bre, &breHandler);
    idt[0x5] = bre;

    // Invalid Opcode exception
    gate_desc_t invop;
    invop.seg_selector = KERNEL_CODE_SEGMENT;
    invop.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(invop, &invopHandler);
    idt[0x6] = invop;

    // Device Not Available exception
    gate_desc_t dna;
    dna.seg_selector = KERNEL_CODE_SEGMENT;
    dna.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(dna, &dnaHandler);
    idt[0x7] = dna;

    // Double Fault exception
    gate_desc_t df;
    df.seg_selector = KERNEL_CODE_SEGMENT;
    df.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(df, &dfHandler);
    idt[0x8] = df;

    // Coprocessor Segment Overrun exception
    gate_desc_t cso;
    cso.seg_selector = KERNEL_CODE_SEGMENT;
    cso.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(cso, &csoHandler);
    idt[0x9] = cso;

    // Invalid TSS exception
    gate_desc_t itss;
    itss.seg_selector = KERNEL_CODE_SEGMENT;
    itss.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(itss, &itssHandler);
    idt[0xa] = itss;

    // Segment Not Present exception
    gate_desc_t snp;
    snp.seg_selector = KERNEL_CODE_SEGMENT;
    snp.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(snp, &snpHandler);
    idt[0xb] = snp;

    // Stack-Segment Fault exception
    gate_desc_t ssf;
    ssf.seg_selector = KERNEL_CODE_SEGMENT;
    ssf.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(ssf, &ssfHandler);
    idt[0xc] = ssf;

    // General Protection exception
    gate_desc_t gp;
    gp.seg_selector = KERNEL_CODE_SEGMENT;
    gp.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(gp, &gpHandler);
    idt[0xd] = gp;

    // Page Fault exception
    gate_desc_t pf;
    pf.seg_selector = KERNEL_CODE_SEGMENT;
    pf.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(pf, &pfHandler);
    idt[0xe] = pf;

    // Math Fault exception
    gate_desc_t mf;
    mf.seg_selector = KERNEL_CODE_SEGMENT;
    mf.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(mf, &mfHandler);
    idt[0x10] = mf;

    // Alignment Check exception
    gate_desc_t ac;
    ac.seg_selector = KERNEL_CODE_SEGMENT;
    ac.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(ac, &acHandler);
    idt[0x11] = ac;

    // Machine Check exception
    gate_desc_t mc;
    mc.seg_selector = KERNEL_CODE_SEGMENT;
    mc.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(mc, &mcHandler);
    idt[0x12] = mc;

    // SIMD Floating-Point exception
    gate_desc_t simdfp;
    simdfp.seg_selector = KERNEL_CODE_SEGMENT;
    simdfp.dpl = 0; //privilege level =0 (kernel)
    SET_IDT_ENTRY(simdfp, &simdfpHandler);
    idt[0x12] = simdfp;
}

/* If want to make umbrella function, you can pull the exception number from the 
* top of the stack. The problem lies in finding the top of the stack at this point.*/
