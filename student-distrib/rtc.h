#ifndef RTC_H
#define RTC_H

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

volatile int rtc_service;
// initialize rtc
void rtc_init(void);

// real interrupt handler for rtc
void rtc_interrupt(void);

int rtc_read();
int rtc_close();
int rtc_write(unsigned int frequency);
int rtc_open();

#endif
