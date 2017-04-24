#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "isr_wrappers.h"
#include "filesys.h"
#include "rtc.h"
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_SCANCODE_PORT 0x60
#define KEYBOARD_IRQ 1

#define KEY_NUM 128
#define BUFFER_SIZE 128
#define CASE_NUM 4
#define CASE_REG 0
#define CASE_SHIFT 2
#define CASE_CAPS 1
#define CASE_BOTH 3
#define KEY_EMPTY '\0'
#define CAPS 0x3A
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LEFT_CURSOR 0x4B
#define RIGHT_CURSOR 0x4D
#define UP_CURSOR 0x48
#define DOWN_CURSOR 0x50
#define F1 0x3B
#define F2 0x3C
#define F3 0x3D
#define LEFT_SHIFT_PRESSED 0x2A
#define RIGHT_SHIFT_PRESSED 0x36
#define CTRL_PRESSED 0x1D
#define ALT_PRESSED 0x38
#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_RELEASED 0xB6
#define CTRL_RELEASED 0x9D
#define ALT_RELEASED 0xB8


#define RTC_INI_FRQ 2
#define FS_IDX 1
#define LINE_END '\n'

extern volatile unsigned char* buffer_key;
extern volatile int* enter_flag;
extern volatile int* buffer_idx;
extern volatile int* cursor_x;
extern volatile int* cursor_y;

// initialize keyboard interrupt handler
extern void keyboard_init(void);

// real keyboard interrupt handler
extern void keyboard_interrupt(void);

// handle different keyboard functionality
void handle_press(unsigned char scancode);




#endif
