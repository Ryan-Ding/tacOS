#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "isr_wrappers.h"

#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_SCANCODE_PORT 0x60
#define KEYBOARD_IRQ 1


// initialize keyboard interrupt handler
extern void keyboard_init(void);

// real keyboard interrupt handler
extern void keyboard_interrupt(void);




#endif
