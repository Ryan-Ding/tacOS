/* keyboard.c
 this file handles interrupt for keyboard driver
 */


#include "keyboard.h"


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

    if( scancode == 0x24) { printf("J pressed \n"); }
    // printk("Scan Code %x %s.\n",
    //        (int) *((char *) scancode) & 0x7F,
    //        *((char *) scancode) & 0x80 ? "Released" : "Pressed");

    send_eoi(KEYBOARD_IRQ);

    //enable interrupt from this device
    sti();

}
