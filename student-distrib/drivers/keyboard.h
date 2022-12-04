#pragma once


#define KB_IRQ 0x1

void keyboard_handler();
void keyboard_init();
int32_t gets(char * buffer, int nbytes);
