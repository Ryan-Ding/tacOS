#include "idt.h"



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
	Page_Fault
};

void System_Call_Interrupt()
{
    printf("System call\n");
    while (1)
    {}
}

//0
void Divide_Error_Exception()
{
	printf("Divide Error\n");
	 while (1)
	 {}
}
//1
void Debug_Exception ()
{
	printf("Debug\n");
	while (1)
	 {}
}
//2
void NMI_Interupt()
{
	printf("NMI interrupt\n");
	while (1)
	 {}
}
//3
void Breakpoint_Exception()
{
	printf("Breakpoint exception\n");
	while (1)
	 {}
}
//4
void Overflow_Exception()
{
	printf("Overflow Exception\n");
	while (1)
	 {}
}
//5
void BOUND()
{
	printf("BOUND\n");
	while (1)
	 {}
}
//6
void Invalid_Opcode()
{
	printf("Invalid Opcode\n");
	while (1)
	 {}
}
//7
void Device_Not_Available()
{
	printf("Device Not Avaliable\n");
	while (1)
	 {}
}
//8
void Double_Fault()
{
	printf("Double Fault\n");
	while (1)
	 {}
}
//9
void Coprocessor_Segment()
{
	printf("Coprocessor Segment\n");
	while (1)
	 {}
}
//10
void Invalid_TSS()
{
	printf("Invalid TSS\n");
	while (1)
	 {}
}
//11
void Segment_Not_Present()
{
	printf("Segment_Not_Present\n");
	while (1)
	 {}
}
//12
void Stack_Fault()
{
	printf("Stack Fault\n");
	while (1)
	 {}
}
//13
void General_Protection()
{
	printf("General Protection\n");
	while (1)
	 {}
}
//14
void Page_Fault()
{
	printf("Page Fault\n");
	while (1)
	 {}
}
//15


void set_gate(int gate, unsigned type, void* addr, unsigned dpl, unsigned seg){
	idt[gate].present = 1;
	idt[gate].dpl = dpl;
	idt[gate].seg_selector = seg;
    idt[gate].size = 1;
	switch (type) {
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
	SET_IDT_ENTRY(idt[gate], addr);
}


void set_intr_gate(unsigned int n, void* addr) {
	set_gate(n, GATE_INTERRUPT, addr, 0, KERNEL_CS);
}

void set_system_gate(unsigned int n, void* addr) {
	set_gate(n, GATE_SYSTEM, addr, 3, KERNEL_CS);
}
void set_system_intr_gate(unsigned int n, void* addr) {
	set_gate(n, GATE_SYSTEM, addr, 3, KERNEL_CS);	
}
void set_trap_gate(unsigned int n, void* addr) {
	set_gate(n, GATE_TRAP, addr, 0, KERNEL_CS);		
}


void idt_init() {
	int i; 
	for (i = 0; i < NUM_EXCEPTION; ++i ) {
		set_trap_gate(i, exception_handler[i]);
	}
    set_system_intr_gate(0x80,System_Call_Interrupt);

}






