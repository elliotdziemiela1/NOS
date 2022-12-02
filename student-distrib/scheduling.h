#pragma once

#include "lib.h"
#include "./drivers/terminal.h"

#define TOTAL_TERMINALS 3

int8_t get_next_scheduled_pid();

uint8_t displaying_terminal_switch(uint8_t terminalIdx);

void schedule_context_switch();

int8_t get_terminal_active_pid(uint8_t terminalNum);

uint32_t get_terminal_vram(uint8_t terminalNum);

