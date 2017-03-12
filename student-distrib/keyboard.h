#ifndef KEYBOARD_H
#define KEYBOARD_H


#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_SCANCODE_PORT 0x60
#define KEYBOARD_IRQ 1


extern void keyboard_init(void);

extern void keyboard_interrupt_handler(void);

#endif
