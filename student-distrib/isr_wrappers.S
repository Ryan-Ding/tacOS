/* filename: isr_wrapper.s */
.globl   keyboard_interrupt_handler
.align   4
 
keyboard_interrupt_handler:
    pushad
    cld /* C code following the sysV ABI requires DF to be clear on function entry */
    call interrupt_handler
    popad
    iret

