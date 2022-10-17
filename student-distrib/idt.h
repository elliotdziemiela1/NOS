#include "x86_desc.h"
#include "types.h"
#define KERNEL_CODE_SEGMENT 0x0

// ptr = address of handler function to insert into IDT.
void insert_handler(int irqNum, uint32_t ptr, int dpl); // REMEMBER TO CALL THIS IN KERNEL.c

void dummyrtcHandler();
void dummykbHandler();
void sysCallHandler();
void divZeroHandler();
void resHandler();
void nmiHandler();
void bpHandler();
void ofHandler();
void breHandler();
void invopHandler();
void dnaHandler();
void dfHandler();
void csoHandler();
void itssHandler();
void snpHandler();
void ssfHandler();
void gpHandler();
void pfHandler();
void mfHandler();
void acHandler();
void mcHandler();
void simdfpHandler();

void init_idt();
