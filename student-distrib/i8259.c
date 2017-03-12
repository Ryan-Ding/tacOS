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

uint8_t master_mask = 0xFF; /* IRQs 0-7 */
uint8_t slave_mask = 0xFF; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/*
    Reference: http://wiki.osdev.org/8259_PIC
 */
void
i8259_init(void)
{
    outb(ICW1,MASTER_CMD);
    outb(ICW1,SLAVE_CMD);

    outb(ICW2_MASTER,MASTER_DATA);
    outb(ICW2_SLAVE, SLAVE_DATA);

    outb(ICW3_MASTER,MASTER_DATA);
    outb(ICW3_SLAVE ,SLAVE_DATA);

    outb(ICW4,MASTER_DATA);
    outb(ICW4,SLAVE_DATA);

    outb(master_mask,MASTER_DATA);
    outb(slave_mask,SLAVE_DATA);

    enable_irq(SLAVE_INTERRUPT_NUM);// slave start at irq2


}

/* Enable (unmask) the specified IRQ */
/*
    Reference: http://wiki.osdev.org/8259_PIC
 */
void
enable_irq(uint32_t irq_num)
{
    uint16_t port;
    uint8_t value;
    
    if (irq_num >= 0 && irq_num <= 7) {
        //0000 0001 -> 0000 0010
        value = inb(MASTER_DATA) & (~(1 << irq_num));
        port = MASTER_DATA;
        // master_mask |= mask; 
    }
    else if (irq_num>=8 && irq_num <= 15){
        value = inb(SLAVE_DATA) & (~(1 << (irq_num - 8)));
        port = SLAVE_DATA;
    }
    outb(value, port);
}

/* Disable (mask) the specified IRQ */
/*
    Reference: http://wiki.osdev.org/8259_PIC
 */
void
disable_irq(uint32_t irq_num)
{
     uint16_t port;
    uint8_t value;
    
    if (irq_num >= 0 && irq_num <= 7) {
        //0000 0001 -> 0000 0010
        value = inb(MASTER_DATA) | (1 << irq_num);
        port = MASTER_DATA;
        // master_mask |= mask; 
    }
    else if (irq_num>=8 && irq_num <= 15){
        value = inb(SLAVE_DATA) | (1 << (irq_num - 8));
        port = SLAVE_DATA;
    }
    outb(value, port);
}


/* Send end-of-interrupt signal for the specified IRQ
 * EOI OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished
 */
void
send_eoi(uint32_t irq_num)
{
    if (irq_num >= 8) {
        outb(EOI | (irq_num - 8), MASTER_CMD );
        outb(EOI | SLAVE_INTERRUPT_NUM, SLAVE_CMD);
    } else {
        outb(EOI | irq_num, MASTER_CMD);
    }
}
