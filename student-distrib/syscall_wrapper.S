#include "sys_call.h"
#include "syscallnum.h"
#define DO_CALL(name,number)   \
.GLOBL name                   ;\
name:   PUSHL	%EBX          ;\
	MOVL	$number,%EAX  ;\
	MOVL	8(%ESP),%EBX  ;\
	MOVL	12(%ESP),%ECX ;\
	MOVL	16(%ESP),%EDX ;\
	INT	$0x80         ;\
	POPL	%EBX          ;\
	RET



DO_CALL(halt_asm,SYS_HALT)
DO_CALL(execute_asm,SYS_EXECUTE)
DO_CALL(read_asm,SYS_READ)
DO_CALL(write_asm,SYS_WRITE)
DO_CALL(open_asm,SYS_OPEN)
DO_CALL(close_asm,SYS_CLOSE)


.sys_call_general_handler:
    andl %EAX, %EAX
    jle invalid_syscall_num
    cmpl %EAX, $10 
    jg invalid_syscall_num
    # start to call 



    jmp end_of_sys_call


invalid_syscall_num:

end_of_sys_call:    

syscall_jump_table:
    .long halt_asm,execute_asm,read_asm,write_asm,open_asm,close_asm







