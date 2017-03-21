#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "isr_wrappers.h"

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

#define LEFT_SHIFT_PRESSED 0x2A
#define RIGHT_SHIFT_PRESSED 0x36
#define CTRL_PRESSED 0x1D

#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_RELEASED 0xB6
#define CTRL_RELEASED 0x9D

extern volatile unsigned char* buffer_key;
extern volatile int buffer_idx;


// initialize keyboard interrupt handler
extern void keyboard_init(void);

// real keyboard interrupt handler
extern void keyboard_interrupt(void);

void handle_press(unsigned char scancode);




#endif
