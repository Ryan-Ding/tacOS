/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"


/* Interrupt masks to determine which interrupts
 * are enabled and disabled */

uint8_t master_mask =EIGHT_BIT_MASK; /* IRQs 0-7 */
uint8_t slave_mask = EIGHT_BIT_MASK; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/*
    Reference: http://wiki.osdev.org/8259_PIC
 */

/*
 * i8259_init
 * input: NONE
 * description: This function initialize the master and slave 8259 chips
 */

void
i8259_init(void)
{
    // send different ICWs to corresponding place for slave and master

    outb(ICW1,MASTER_CMD);
    outb(ICW1,SLAVE_CMD);

    outb(ICW2_MASTER,MASTER_DATA);
    outb(ICW2_SLAVE, SLAVE_DATA);

    outb(ICW3_MASTER,MASTER_DATA);
    outb(ICW3_SLAVE ,SLAVE_DATA);

    outb(ICW4,MASTER_DATA);
    outb(ICW4,SLAVE_DATA);

    outb(master_mask,MASTER_DATA); // initialize the mask for master and slave
    outb(slave_mask,SLAVE_DATA);

    enable_irq(SLAVE_INTERRUPT_NUM);// slave is designed to start at line 2


}

/*
 * enable_irq
 * input: number of the interrupt location on 8259
 * description: Enable (unmask) the specified IRQ
 */

void
enable_irq(uint32_t irq_num)
{
    uint16_t port;
    uint8_t value;

    if (irq_num >= 0 && irq_num < MASTER_SIZE) {
        value = inb(MASTER_DATA) & (~(1 << irq_num));
        port = MASTER_DATA;
    }
    else if (irq_num>=MASTER_SIZE && irq_num <= SLAVE_SIZE){
        value = inb(SLAVE_DATA) & (~(1 << (irq_num - MASTER_SIZE)));
        port = SLAVE_DATA;
    }
    outb(value, port);
}

/*
 * disable_irq
 * input: number of the interrupt location on 8259
 * description: disable (mask) the specified IRQ
 */

void
disable_irq(uint32_t irq_num)
{
     uint16_t port;
    uint8_t value;
// master
    if (irq_num >= 0 && irq_num <MASTER_SIZE) {
        value = inb(MASTER_DATA) | (1 << irq_num);
        port = MASTER_DATA;
    }
    // slave
    else if (irq_num>=MASTER_SIZE && irq_num <= SLAVE_SIZE){
        value = inb(SLAVE_DATA) | (1 << (irq_num - MASTER_SIZE));
        port = SLAVE_DATA;
    }
    outb(value, port);
}


/*
 * send_eoi
 * Input: number of the interrupt location on 8259
 * Description: Send end-of-interrupt signal for the specified IRQ
 * EOI OR'd with the interrupt number and sent out to the PIC
 * to declare the interrupt finished
 */

void
send_eoi(uint32_t irq_num)
{
    if (irq_num >= MASTER_SIZE) {
        outb(EOI | (irq_num - MASTER_SIZE), SLAVE_CMD);
        outb(EOI | SLAVE_INTERRUPT_NUM, MASTER_CMD);
    } else {
        outb(EOI | irq_num, MASTER_CMD);
    }
}
