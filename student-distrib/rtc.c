
/* rtc.c
 * this file handles the interrupt from the rtc driver
 * for reference : wiki.osdev.org/RTC
 */

// the function turns the IRQ with default 1024 Hz

# include "lib.h"
# include "rtc.h"
# include "i8259.h"

void
rtc_init(void){
    unsigned char prev;
    
    cli();
    
    // select register b and disable NMI
    outb(STATUS_REGISTER_B, NMI_PORT);
    
    //read current value of Register B
    prev = inb(CMOS_PORT);
    
    // set index again
    outb(STATUS_REGISTER_B,NMI_PORT);
    
    // turn on bit 6 of register b
    outb(prev|0x40,CMOS_PORT);
    
    sti();
}

void
rtc_interrupt_handler(void){
    cli();
    
    outb(STATUS_REGISTER_C,NMI_PORT);
    inb(CMOS_PORT);
    
    // test rtc interrupts ???? call lib.c
    //test_interrupts();
    
    send_eoi(RTC_IRQ);
    
    sti();
}
