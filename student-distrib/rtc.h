#ifndef RTC_H
#define RTC_H

#define STATUS_REGISTER_A 0x8A
#define STATUS_REGISTER_B 0x8B
#define STATUS_REGISTER_C 0x0C

#define NMI_PORT 0x70
#define CMOS_PORT 0x71
#define RTC_IRQ 8

// initialize rtc
void rtc_init(void);

// real interrupt handler for rtc
void rtc_interrupt(void);

#endif
