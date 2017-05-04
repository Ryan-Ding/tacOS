#ifndef MOUSE_H
#define MOUSE_H

#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "isr_wrappers.h"
#include "terminal.h"

#define MOUSE_STATUS_PORT 0x64
#define MOUSE_SCANCODE_PORT 0x60
#define MOUSE_IRQ 12
#define MOUSE_SCALE 127
#define Y_SCALE 3
#define X_SCALE 9
#define SIGN_EXT 0xFFFFFF00
#define YO_BIT 0x80
#define XO_BIT 0x40
#define DEFAULT 0xF6
#define ENABLE_DATA 0xF4
#define TIME_OUT 100000
#define BEFORE_FLAG 0xD4
#define ENABLE_FLAG_FIRST 0x20
#define ENABLE_FLAG_SECOND 0x60
#define AUX_FLAG 0xA8
// initialize keyboard interrupt handler
extern void mouse_init(void);

// real keyboard interrupt handler
extern void mouse_interrupt(void);

extern volatile uint8_t first_start_flag[3];
// handle different keyboard functionality

void mouse_wait(uint8_t type);
void mouse_write(uint8_t word);
uint8_t mouse_read(void );
//
// extern uint32_t delta_y;
// extern uint8_t delta_x;
#endif
