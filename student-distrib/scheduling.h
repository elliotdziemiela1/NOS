#pragma once

#include "lib.h"
#include "./drivers/terminal.h"

#define TOTAL_TERMINALS 3

uint8_t displaying_terminal_switch(uint8_t terminalIdx);

void schedule_context_switch();

