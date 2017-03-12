/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* * Reference
 *:ftp://ftp.csc.fi/pub/Linux/kernel/v2.4/patch-html/patch-2.4.6/linux_arch_mips_kernel_i8259.c.html
 */

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
// a bit redundant but whatever
uint8_t master_mask = 0xFF; /* IRQs 0-7 */
uint8_t slave_mask = 0xFF; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
    outb(ICW1,MASTER_8259_PORT);
    outb(ICW1,SLAVE_8259_PORT);
    
    outb(ICW2_MASTER,MASTER_8259_PORT+1);
    outb(ICW2_SLAVE, SLAVE_8259_PORT+1);

    outb(ICW3_MASTER,MASTER_8259_PORT+1);
    outb(ICW3_SLAVE ,SLAVE_8259_PORT+1);

    outb(ICW4,MASTER_8259_PORT+1);
    outb(ICW4,SLAVE_8259_PORT+1);
    
    enable_irq(2);// initialize with an used irq #2

    
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
    // 1111 1110 for irq
    uint8_t mask = 0xFE;
    int i;
    
    if (irq_num>=0 && irq_num<=7) {
        //1111 1110 -> 1111 1101
        for (i =0; i<irq_num; i++)
            mask= mask<<1+1;
        outb(master_mask & mask, MASTER_8259_PORT+1);
    }
    else if (irq_num>=8 && irq_num<=15){
        for (i =0; i<irq_num-8; i++)
            mask = mask<<1+1;
        outb(slave_mask & mask, SLAVE_8259_PORT+1);
    }
        
    
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
    // 0000 0001 for irq
    uint8_t mask = 0x01;
    int i;
    
    if (irq_num>=0 && irq_num<=7) {
        //0000 0001 -> 0000 0010
        for (i =0; i<irq_num; i++)
            mask= mask<<1;
        
        outb(master_mask | mask, MASTER_8259_PORT+1);
    }
    else if (irq_num>=8 && irq_num <=15){
        for (i =0; i<irq_num-8; i++)
            mask = mask<<1;
        
        outb(slave_mask | mask, SLAVE_8259_PORT+1);
    }

}


/* Send end-of-interrupt signal for the specified IRQ 
 * EOI OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished
 */
void
send_eoi(uint32_t irq_num)
{
    if (irq_num>=0 && irq_num<=7) {
        outb(EOI|irq_num,MASTER_8259_PORT);
    }
    
    else if (irq_num >=8 && irq_num <=15){
        outb (EOI|irq_num-8,SLAVE_8259_PORT);
        outb(EOI+2,MASTER_8259_PORT);
    }
}

