#include "idt.h"

// listing the exception

void (* exception_handler[NUM_EXCEPTION]) = {
	Divide_Error_Exception,
	Debug_Exception, 
 	NMI_Interupt,
 	Breakpoint_Exception,
 	Overflow_Exception,
 	BOUND,
	Invalid_Opcode,
	Device_Not_Available,
	Double_Fault,
	Coprocessor_Segment,
	Invalid_TSS,
	Segment_Not_Present,
	Stack_Fault,
	General_Protection,
	Page_Fault,
	Reserved_Int,
	Floating_Point,
	Alignment_Check,
	Machine_Check_Exception,
	SIMD_Floating
};

/* 
 * SIMD_Floating
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */

void SIMD_Floating()
{
	printf("SIMD Floating Point Exception\n");
	while (1)
	{}
}

/* 
 * Machine_Check_Exception
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */


void Machine_Check_Exception()
{
	printf("Machine Check Exception\n");
	while (1)
	{}
}

/* 
 * Alignment_Check
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */

void Alignment_Check()
{
	printf("Alignment Check Exception\n");
	while (1)
	{}
}

/* 
 * Floating Point
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */

void Floating_Point()
{
	printf("Floating Point Error\n");
	while(1)
	{}
}

/* 
 * Reserved Int
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */


void Reserved_Int()
{

}

/* 
 * System Call Interrupt
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */

void System_Call_Interrupt()
{
	printf("System call\n");
	while (1)
	{}
}

/* 
 * Divide Error Exception
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */

//0
void Divide_Error_Exception()
{
	printf("Divide Error\n");
	 while (1)
	 {}
}

/* 
 * Debug Exception
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//1
void Debug_Exception ()
{
	printf("Debug\n");
	while (1)
	 {}
}

/* 
 * NMI Interrupt
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//2
void NMI_Interupt()
{
	printf("NMI interrupt\n");
	while (1)
	 {}
}

/* 
 * Breakpoint Exception
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//3
void Breakpoint_Exception()
{
	printf("Breakpoint exception\n");
	while (1)
	 {}
}

/* 
 * Overflow Exception
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//4
void Overflow_Exception()
{
	printf("Overflow Exception\n");
	while (1)
	 {}
}

/* 
 * Bound
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//5
void BOUND()
{
	printf("BOUND\n");
	while (1)
	 {}
}

/* 
 * Invalid Opcode
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//6
void Invalid_Opcode()
{
	printf("Invalid Opcode\n");
	while (1)
	 {}
}

/* 
 * Device Not Available
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//7
void Device_Not_Available()
{
	printf("Device Not Avaliable\n");
	while (1)
	 {}
}

/* 
 * Double Fault
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//8
void Double_Fault()
{
	printf("Double Fault\n");
	while (1)
	 {}
}

/* 
 * Coprocessor Segment
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//9
void Coprocessor_Segment()
{
	printf("Coprocessor Segment\n");
	while (1)
	 {}
}

/* 
 * Invalid TSS
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//10
void Invalid_TSS()
{
	printf("Invalid TSS\n");
	while (1)
	 {}
}

/* 
 * Segment Not Present
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//11
void Segment_Not_Present()
{
	printf("Segment_Not_Present\n");
	while (1)
	 {}
}

/* 
 * Stack Fault
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//12
void Stack_Fault()
{
	printf("Stack Fault\n");
	while (1)
	 {}
}

/* 
 * General Protection
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//13
void General_Protection()
{
	printf("General Protection\n");
	while (1)
	 {}
}

/* 
 * Page Fault
 * input: NONE
 * description: Print the blue screen
 * side effect: Error Message is printed
 */
//14
void Page_Fault()
{
	printf("Page Fault\n");

	uint32_t cr2_temp;
	asm volatile(
		"movl  %%cr2, %0;"
		: "=r" (cr2_temp)
		:
	);
	printf("Page Fault when trying to access %x\n",cr2_temp);

	system_execute("shell");
}
//15
/* 
 * set_gate
 * input: gate: gate number
 *		  type: gate type
 *		  addr: handler address
 *		  dpl: dpl
 *		  seg: segment selector
 * description: interrupt handler to set up idt table
 * side effect: idt is set
 */


void set_gate(int gate, unsigned type, void* addr, unsigned dpl, unsigned seg){
	idt[gate].present = 1;	//set present value
	idt[gate].dpl = dpl;	//set dpl
	idt[gate].seg_selector = seg;	//set segment
    idt[gate].size = 1;		//set size
	switch (type) { //set reserved value depending on the gate type
		case GATE_INTERRUPT:
			idt[gate].reserved0 = 0;
			idt[gate].reserved1 = 1;
			idt[gate].reserved2 = 1;
			idt[gate].reserved3 = 0;
			idt[gate].reserved4 = 0;
			break;
		case GATE_SYSTEM:
			idt[gate].reserved0 = 0;
			idt[gate].reserved1 = 1;
			idt[gate].reserved2 = 1;
			idt[gate].reserved3 = 1;
			idt[gate].reserved4 = 0;
			break;
		case GATE_TRAP:
			idt[gate].reserved0 = 0;
			idt[gate].reserved1 = 1;
			idt[gate].reserved2 = 1;
			idt[gate].reserved3 = 1;
			idt[gate].reserved4 = 0;			
			break;
	}
	SET_IDT_ENTRY(idt[gate], addr);	//set offset of corresponding idt
}

/* 
 * set_intr_gate
 * input: n:gate number; addr: handler address
 * description: initialize and set interrupt gate
 * side effect: idt gate will be set to be ready for interrupt call
 */
void set_intr_gate(unsigned int n, void* addr) {
	set_gate(n, GATE_INTERRUPT, addr, IDL0, KERNEL_CS);
}

/* 
 * set_system_gate
 * input: n:gate number; addr: handler address
 * description: initialize and set system gate
 * side effect: idt gate will be set to be ready for system call
 */

void set_system_gate(unsigned int n, void* addr) {
	set_gate(n, GATE_SYSTEM, addr, IDL3, KERNEL_CS);
}

/* 
 * set_system_intr_gate
 * input: n:gate number; addr: handler address
 * description: initialize and set system interrupt gate
 * side effect: idt gate will be set to be ready for system interrupt call
 */
void set_system_intr_gate(unsigned int n, void* addr) {
	set_gate(n, GATE_SYSTEM, addr, IDL3, KERNEL_CS);	
}

/* 
 * set_trap_gate
 * input: n:gate number; addr: handler address
 * description: initialize and set system gate
 * side effect: idt gate will be set to be ready for system call
 */
void set_trap_gate(unsigned int n, void* addr) {
	set_gate(n, GATE_TRAP, addr, IDL3, KERNEL_CS);		
}

/* 
 * idt_init
 * description: initialize interrupt descriptor table
 * side effect: this function will handle the initializatio of different kinds of idts
 */
void idt_init() {
	int i; 
	for (i = 0; i < NUM_EXCEPTION; ++i ) {
		if (i != RESERVE_EXC) //Ignore the reserved exception
			set_trap_gate(i, exception_handler[i]);
	}
	set_system_intr_gate(SYSTEM_INT,sys_call_general_handler);
}
