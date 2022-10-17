#include "lib.h"
#include "synch.h"
#include "idt.h"
#include "i8259.h"

#define KB_DATAPORT 0x60
#define KB_IRQ 0x1

char scanTable[0xff];

void keyboard_init(){
    insert_handler(KB_IRQ,&keyboard_handler);
    char tmpTable[0xff] = { '!' };
    tmpTable[0x02] = '1';
    tmpTable[0x03] = '2';
    tmpTable[0x04] = '3';
    tmpTable[0x05] = '4';
    tmpTable[0x06] = '5';
    tmpTable[0x07] = '6';
    tmpTable[0x08] = '7';
    tmpTable[0x09] = '8';
    tmpTable[0x0a] = '9';
    tmpTable[0x0b] = '0';
    tmpTable[0x10] = 'q';
    tmpTable[0x11] = 'w';
    tmpTable[0x12] = 'e';
    tmpTable[0x13] = 'r';
    tmpTable[0x14] = 't';
    tmpTable[0x15] = 'y';
    tmpTable[0x16] = 'u';
    tmpTable[0x17] = 'i';
    tmpTable[0x18] = 'o';
    tmpTable[0x19] = 'p';
    tmpTable[0x1e] = 'a';
    tmpTable[0x1f] = 's';
    tmpTable[0x20] = 'd';
    tmpTable[0x21] = 'f';
    tmpTable[0x22] = 'g';
    tmpTable[0x23] = 'h';
    tmpTable[0x24] = 'j';
    tmpTable[0x25] = 'k';
    tmpTable[0x26] = 'l';
    tmpTable[0x2c] = 'z';
    tmpTable[0x2d] = 'x';
    tmpTable[0x2e] = 'c';
    tmpTable[0x2f] = 'v';
    tmpTable[0x30] = 'b';
    tmpTable[0x31] = 'n';
    tmpTable[0x32] = 'm';

    scanTable = tmpTable;
}



void keyboard_handler(){
    cli();
    disable_irq(KB_IRQ);
    uint8_t tmp = inb(KB_DATAPORT);
    printf(scanTable[tmp]);
    send_eoi(KB_IRQ);
    enable_irq(KB_IRQ);
    sti();
}

