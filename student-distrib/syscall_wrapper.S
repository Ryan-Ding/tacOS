
sys_call_general_handler:
	cmpl $0, %EAX
    jle invalid_syscall_num
    cmpl $10,%eax
    jg invalid_syscall_num
    pushl %EBP
    pushl %ESI
    pushl %EDI
    pushl %EBX
    # start to call
	pushl %EDX
	pushl %ECX
	pushl %EBX
	call *syscall_jump_table(, %eax, 4)
	addl  $12,%esp
	popl  %EBX
	popl  %EDI
	popl  %ESI
	popl  %EBP
    iret

invalid_syscall_num:
	xorl 	%eax,%eax
 	decl 	%eax
 	iret

syscall_jump_table:
    .long 0x0, system_halt, system_execute,system_read,system_write,system_open,system_close


.global sys_call_general_handler
