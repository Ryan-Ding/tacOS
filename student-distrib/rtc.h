#ifndef RTC_H
#define RTC_H

#include "pcb.h"
#include "sys_call.h"

#define STATUS_REGISTER_A 0x8A
#define STATUS_REGISTER_B 0x8B
#define STATUS_REGISTER_C 0x0C

#define NMI_PORT 0x70
#define CMOS_PORT 0x71
#define RTC_IRQ 8
#define OPEN_FLAG 0x06
#define OFF_FLAG 0
#define INB_MASK 0xF0
#define RATE_MASK 0x0F
#define BIT_SIX_MASK 0x40

#define HIGHEST_BIT_MASK 8192
#define MIN_FREQUENCY 3


// initialize rtc
void rtc_init(void);

// real interrupt handler for rtc
void rtc_interrupt(void);


int rtc_stop();

int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int rtc_close(int32_t fd);
int rtc_write(unsigned int frequency);
int rtc_write_syscall(int32_t fd, const void* buf, int32_t nbytes);
int rtc_open();
int rtc_open_syscall(const uint8_t* filename);

extern pcb_t* curr_process;

#endif
