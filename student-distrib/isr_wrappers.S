/* filename: isr_wrapper.s */

#define ASM     1
#include "x86_desc.h"

.globl   keyboard_interrupt_handler
.globl   rtc_interrupt_handler
.align   4

 
keyboard_interrupt_handler:
    pushal
    cld /* C code following the sysV ABI requires DF to be clear on function entry */
    call keyboard_interrupt
    popal
    iret

rtc_interrupt_handler:
    pushal
    cld /* C code following the sysV ABI requires DF to be clear on function entry */
    call rtc_interrupt
    popal
    iret
