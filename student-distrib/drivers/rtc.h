#pragma once
#define RTC_IRQ 0x8
void rtc_init();
void rtc_handler();

int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);
int32_t set_frequency(int32_t freq);
int is_power_of_two(int num);
