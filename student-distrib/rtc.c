
/* rtc.c
 * this file handles the interrupt from the rtc driver
 * for reference : wiki.osdev.org/RTC
 */

// the function turns the IRQ with default 1024 Hz

# include "lib.h"
# include "rtc.h"
# include "i8259.h"
# include "isr_wrappers.h"
# include "idt.h"

/*
 * rtc_init
 * input: NONE
 * description: This function initialize interrupt handler for rtc
 */




void
rtc_init(void){
    unsigned char prev;
    
    cli();

 

    set_intr_gate(RTC_IRQ + SLAVE_IDT_OFFSET - MASTER_SIZE, rtc_interrupt_handler);
    
    // select register b and disable NMI
    outb(STATUS_REGISTER_B, NMI_PORT);
    
    //read current value of Register B
    prev = inb(CMOS_PORT);
    
    // set index again
    outb(STATUS_REGISTER_B,NMI_PORT);
    
    // turn on bit 6 of register b
    outb(prev|0x40,CMOS_PORT);
    
    enable_irq(RTC_IRQ);
    rtc_service = 0;
    sti();
}


void 
rtc_open(void)
{

    unsigned char rate = OPEN_FLAG;
    cli();
    outb(STATUS_REGISTER_A,NMI_PORT);
    char prev = inb(CMOS_PORT);
    outb(STATUS_REGISTER_A,NMI_PORT);
    outb((prev & INB_MASK)|rate, CMOS_PORT);
    sti();
}
/*
void
rtc_close(void)
{
    unsigned char rate = OFF_FLAG;
    cli();
    outb(STATUS_REGISTER_A,NMI_PORT);
    char prev = inb(CMOS_PORT);
    outb(STATUS_REGISTER_A,NMI_PORT);
    outb((prev & INB_MASK)|rate, CMOS_PORT);
    sti();
}
*/

void rtc_read()
{
    while (rtc_service)
    {}

    

}



int
rtc_write(unsigned int frequency)
{
    unsigned char rate = 0;
    if (frequency > HIGHEST_BIT_MASK)
        return -1;

    if (frequency == 0)
        rate = 0;
    else
    {
        if (frequency & (frequency - 1) != 0)
                return -1;

        rate = MIN_FREQUENCY;
        while (frequency & HIGHEST_BIT_MASK == 0)
            {
                rate++;
                frequency<<1;
            }

    }
    


    cli();
    outb(STATUS_REGISTER_A,NMI_PORT);
    char prev = inb(CMOS_PORT);
    outb(STATUS_REGISTER_A,NMI_PORT);
    outb((prev & INB_MASK)|rate, CMOS_PORT);
    sti();
    return 0;

}


/*
 * rtc_interrupt
 * input: NONE
 * description: This function handles interrupt from the rtc
 */

void
rtc_interrupt(void){
    cli();
    rtc_service = 1;
    outb(STATUS_REGISTER_C,NMI_PORT); // select register c
    inb(CMOS_PORT); // throw away contents

    printf("b \n");
    //test_interrupts();
    send_eoi(RTC_IRQ);
    rtc_service = 0;
    sti();
}
