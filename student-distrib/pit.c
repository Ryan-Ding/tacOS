#include "pit.h"


void pit_init(){
    int hz = 1;

    enable_irq(PIT_IRQ);
    set_intr_gate(PIT_IRQ + MASTER_IDT_OFFSET, (void*) pit_interrupt_handler);
    outb(0x36,PIT_MODE);
    outb((PIT_SCALE / hz) & MASK_LOWER,PIT_CHANNEL0);
    outb(((PIT_SCALE / hz) >> ONE_BYTE) & MASK_LOWER,PIT_CHANNEL0);


}


void pit_interrupt(){
  cli();
  // asm volatile("pushal;"
  //              "pushfl;"
  //              :::"memory", "cc"
  //            );
  //
  switch_task();
  //
  // asm volatile("popal;"
  //              "popfl;"
  //              :::"memory", "cc"
  //            );
  send_eoi(PIT_IRQ);
  sti();

}
