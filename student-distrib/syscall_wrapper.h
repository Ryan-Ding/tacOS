// #include "sys_call.h"
#include "syscallnum.h"
#include "sys_call.h"

extern uint32_t sys_call_general_handler();



// open_asm:
// 		pushl %EBX
// 		movl 	0x10(%ESP),%EDX
// 		movl 	0x0C(%ESP),%ECX
// 		movl 	0x08(%ESP),%EBX
// 		movl  $0x05,%EAX
// 		int 	$0x80
// 		jmp 	end_of_sys_call

// halt_asm:
// 		pushl %EBX
// 		movl 	0x10(%ESP),%EDX
// 		movl 	0x0C(%ESP),%ECX
// 		movl 	0x08(%ESP),%EBX
// 		movl  $0x01,%EAX
// 		int 	$0x80
// 		jmp 	end_of_sys_call

// execute_asm:
// 		pushl %EBX
// 		movl 	0x10(%ESP),%EDX
// 		movl 	0x0C(%ESP),%ECX
// 		movl 	0x08(%ESP),%EBX
// 		movl  $0x02,%EAX
// 		int 	$0x80
// 		jmp 	end_of_sys_call

// read_asm:
// 		pushl %EBX
// 		movl 	0x10(%ESP),%EDX
// 		movl 	0x0C(%ESP),%ECX
// 		movl 	0x08(%ESP),%EBX
// 		movl  $0x03,%EAX
// 		int 	$0x80
// 		jmp 	end_of_sys_call

// write_asm:
// 		pushl %EBX
// 		movl 	0x10(%ESP),%EDX
// 		movl 	0x0C(%ESP),%ECX
// 		movl 	0x08(%ESP),%EBX
// 		movl  $0x04,%EAX
		
// 		int 	$0x80
// 		jmp 	end_of_sys_call

// close_asm:
// 		pushl %EBX
// 		movl 	0x10(%ESP),%EDX
// 		movl 	0x0C(%ESP),%ECX
// 		movl 	0x08(%ESP),%EBX
// 		movl  	$0x05,%EAX
// 		int 	$0x80
// 		jmp 	end_of_sys_call

