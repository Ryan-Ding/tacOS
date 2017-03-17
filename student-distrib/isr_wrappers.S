/*  isr_wrapper.s

    This file implement the Interrupt Service Routine
    so that the system know which isr to call while a certain interrupt occurs
    
    For reference: wiki.osdev.org/Interrupt_Service_Routines
 */

#define ASM     1
#include "x86_desc.h"

.globl   keyboard_interrupt_handler
.globl   rtc_interrupt_handler
.align   4

// assembly wrapper to call c function to deal with the interrupt

/*  keyboard_interrupt_handler
    description: assembly wrapper to call c function to deal with the interrupt
    side effect: save all the registers before calling interrupt 
 */

keyboard_interrupt_handler:
    pushal
    cld /* C code following the sysV ABI requires DF to be clear on function entry */
    call keyboard_interrupt
    popal
    iret

/*  rtc_interrupt_handler
    description: assembly wrapper to call c function to deal with the interrupt
    side effect: save all the registers before calling rtc interrupt handler
 */

rtc_interrupt_handler:
    pushal
    cld /* C code following the sysV ABI requires DF to be clear on function entry */
    call rtc_interrupt
    popal
    iret
