#include "pit.h"


/*
 * pit_init
 * input: none
 * description: initialize pit
 * return value: none
 * side effect : set hz to 100
 */
void pit_init(){
    int hz = 100;

    enable_irq(PIT_IRQ);
    set_intr_gate(PIT_IRQ + MASTER_IDT_OFFSET, (void*) pit_interrupt);
    outb(PIT_SET,PIT_MODE);
    outb((PIT_SCALE / hz) & MASK_LOWER,PIT_CHANNEL0);
    outb(((PIT_SCALE / hz) >> ONE_BYTE) & MASK_LOWER,PIT_CHANNEL0);


}



/*
 * pit_interrupt
 * input: none
 * description: pit interrupt handler
 * return value: none
 * side effect : call scheduling function
 */
void pit_interrupt(){
  // cli();
  // send_eoi(PIT_IRQ);
  // asm volatile("pushal;"
  //              :::"memory", "cc"
  //            );

  // switch_task();

  // asm volatile("popal;"
  //              :::"memory", "cc"
  //            );
  // sti();

	asm	volatile(
		"cli;"
		"pushal;");

	  send_eoi(PIT_IRQ);

	  switch_task();
	  //printf("im here");

	  asm("popal");
	  sti();

	  asm(
		  "leave;"
		  "iret;");
}
