#pragma once

#define PIT_IRQ         0x0
#define PIT_MODE        0x36
#define PIT_REG_CMD     0x43
#define PIT_INTERVAL    11932
#define PIT_REG_DATA    0x40

//volatile uint32_t pit_timer;

void pit_init();
void pit_handler();

