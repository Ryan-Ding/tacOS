/* keyboard.c 
 this file handles interrupt for keyboard driver 
 */


#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

void
keyboard_init(void){
    enable_irq(KEYBOARD_IRQ);
}

void
keyboard_interrupt_handler(void){
    //disable interrupt from this device
    cli();
    
    unsigned char input;
    while (1) {
        input = inb(KEYBOARD_SCANCODE_PORT);
        if (input>0)
            break;
    }
    printk("Scan Code %x %s.\n",
           (int) *((char *) scancode) & 0x7F,
           *((char *) scancode) & 0x80 ? "Released" : "Pressed");
    
    send_eoi(KEYBOARD_IRQ);
    //enable interrupt from this device
    sti();
    
}