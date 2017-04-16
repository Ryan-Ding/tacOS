
sys_call_general_handler:
	pushal
	pushfl
	cmpl $0, %EAX
    jle invalid_syscall_num
    cmpl $10,%eax
    jg invalid_syscall_num
    # start to call
	pushl %EDX
	pushl %ECX
	pushl %EBX
	call *syscall_jump_table(, %eax, 4)
	popl  %EBX
	popl  %ECX
	popl  %EDX
	popfl
	popal
    jmp end_of_sys_call


invalid_syscall_num:
		xorl 	%eax,%eax
		decl 	%eax
		iret

end_of_sys_call:
		iret

open_asm:
		pushl %EBX
		movl 	0x10(%ESP),%EDX
		movl 	0x0C(%ESP),%ECX
		movl 	0x08(%ESP),%EBX
		movl  $0x05,%EAX
		int 	$0x80
		jmp 	end_of_sys_call

halt_asm:
		pushl %EBX
		movl 	0x10(%ESP),%EDX
		movl 	0x0C(%ESP),%ECX
		movl 	0x08(%ESP),%EBX
		movl  $0x01,%EAX
		int 	$0x80
		jmp 	end_of_sys_call

execute_asm:
		pushl %EBX
		movl 	0x10(%ESP),%EDX
		movl 	0x0C(%ESP),%ECX
		movl 	0x08(%ESP),%EBX
		movl  $0x02,%EAX
		int 	$0x80
		jmp 	end_of_sys_call

read_asm:
		pushl %EBX
		movl 	0x10(%ESP),%EDX
		movl 	0x0C(%ESP),%ECX
		movl 	0x08(%ESP),%EBX
		movl  $0x03,%EAX
		int 	$0x80
		jmp 	end_of_sys_call

write_asm:
		pushl %EBX
		movl 	0x10(%ESP),%EDX
		movl 	0x0C(%ESP),%ECX
		movl 	0x08(%ESP),%EBX
		movl  $0x04,%EAX
		
		int 	$0x80
		jmp 	end_of_sys_call

close_asm:
		pushl %EBX
		movl 	0x10(%ESP),%EDX
		movl 	0x0C(%ESP),%ECX
		movl 	0x08(%ESP),%EBX
		movl  	$0x05,%EAX
		int 	$0x80
		jmp 	end_of_sys_call

syscall_jump_table:
    .long 0x0, system_halt, system_execute,system_read,system_write,system_open,close_asm


.global sys_call_general_handler
