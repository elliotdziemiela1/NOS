#pragma once

#include "lib.h"
#include "./drivers/terminal.h"

#define TOTAL_TERMINALS 3

volatile terminal_t terminals[TOTAL_TERMINALS];
volatile int8_t current_terminal;
volatile int8_t current_displaying_terminal;

int8_t get_next_scheduled_pid();

uint8_t displaying_terminal_switch(uint8_t terminalIdx);

void schedule_context_switch();

