#pragma once
void rtc_init();
void rtc_handler();

int32_t read(int32 t fd, void* buf, int32 t nbytes);
int32_t write(int32 t fd, const void* buf, int32 t nbytes);
int32_t open(const uint8 t* filename);
int32_t close(int32 t fd);
int32_t set_frequency(int32_t freq);
int power_of_two(int num);
