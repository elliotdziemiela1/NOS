#pragma once

#define PIT_IRQ         0
#define PIT_MODE        0x36
#define PIT_REG_CMD     0x43
#define PIT_INTERVAL    11932
#define PIT_REG_DATA    0x40

void pit_init();
void pit_handler();
