#include "pit.h"


void pit_init(){
    int hz = 1;
    cli();
    set_intr_gate(PIT_IRQ + MASTER_IDT_OFFSET,pit_interrupt_handler);
    outb(PIT_SET,PIT_MODE);
    outb((PIT_SCALE / hz) & MASK_LOWER,PIT_CHANNEL0);
    outb(((PIT_SCALE / hz) >> ONE_BYTE) & MASK_LOWER,PIT_CHANNEL0);
    enable_irq(PIT_IRQ);
    sti();

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
