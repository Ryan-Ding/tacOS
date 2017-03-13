/* keyboard.c
 this file handles interrupt for keyboard driver
 */


#include "keyboard.h"

// scancode lookup table used for screen ecoing
unsigned char scancode_set[128] =
{
    '\0',  0x1B, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=',
    0x08, /* backspace */
    0x09, /* tab */
    'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',/* enter */
    0xA2,	/* left Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';','\'', '`',
    0xA0,/* left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/',
    0xA1,/* right shift */
    '*',
    0x12,	/* alt */
    ' ',	/* space */
    0x14,	/* caps lock */
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',/* F1 ... F10 */
    '\0',	/* num lock */
    '\0',	/* scroll Lock */
    '\0', '\0', '\0',	/* keypad 7 8 9 */
    '-',
    '\0', '\0', '\0',	/* keypad 4 5 9 */
    '+',
    '\0', '\0', '\0', '\0', '\0', /* keypad 1 2 3 0 . */
    '\0', '\0', '\0',	/* not used keys */
    '\0', '\0',	/* F11 F12 Key */
    '\0',	/* undefined keys*/
};

/* 
 * keyboard_init
 * input: NONE
 * description: This function initialize interrupt handler for keyboard
 */

void
keyboard_init(void){
    // call wrapper function
    set_intr_gate(KEYBOARD_IRQ + MASTER_IDT_OFFSET, keyboard_interrupt_handler);
    
    // enable keyboard irq line
    enable_irq(KEYBOARD_IRQ);
    
    //printf("keyboard init \n");
}

/*
 * keyboard_interrupt
 * input: NONE
 * description: This function echo the scancode received to the screen
 */

void
keyboard_interrupt(void){
    //printf(" inside keyboard handler \n");


    //disable interrupt from this device
    cli();

    unsigned char scancode;
    
    // get input from keyboard
    while (1) {
        scancode = inb(KEYBOARD_SCANCODE_PORT);
        if (scancode > 0) { break; }
    }

    // only print when it is pressed instead of released
    if( scancode < 0x80) { printf("%c pressed \n",scancode_set[scancode]); }
    

    send_eoi(KEYBOARD_IRQ);

    //enable interrupt from this device
    sti();

}
