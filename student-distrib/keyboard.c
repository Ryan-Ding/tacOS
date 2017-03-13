/* keyboard.c
 this file handles interrupt for keyboard driver
 */


#include "keyboard.h"

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

void
keyboard_init(void){
    set_intr_gate(KEYBOARD_IRQ + MASTER_IDT_OFFSET, keyboard_interrupt_handler);
    enable_irq(KEYBOARD_IRQ);
    printf("keyboard init \n");
}

void
keyboard_interrupt(void){
    printf(" inside keyboard handler \n");

    /*int32_t i;
    for (i=0; i < 80; i++) {
        ((char *)(0xB8000))[i<<1]++;
    }*/

    //disable interrupt from this device
    cli();

    unsigned char scancode;
    while (1) {
        scancode = inb(KEYBOARD_SCANCODE_PORT);
        if (scancode > 0) { break; }
    }

    if( scancode < 0x80) { printf("%c pressed \n",scancode_set[scancode]); }
    //        only print when it is pressed instead of released

    send_eoi(KEYBOARD_IRQ);

    //enable interrupt from this device
    sti();

}
