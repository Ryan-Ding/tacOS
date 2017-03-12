#include "idt.h"
#include "x86_desc.h"
#include "lib.h"


//0
void Divde_Error_EXCEPTION()
{
	printf("Divide Error\n");
	// while (1)
	// {}
}
//1
void Debug_Exception ()
{
	printf("Debug\n");
}
//2
void NMI_Interupt()
{
	printf("NMI interrupt\n");
}
//3
void Breakpoint_Exception()
{
	printf("Breakpoint exception\n");
}
//4
void Overflow_Exception()
{
	printf("Overflow Exception\n");
}
//5
void BOUND()
{
	printf("BOUND\n");
}
//6
void Invalid_Opcode()
{
	printf("Invalid Opcode\n");
}
//7
void Device_Not_Available()
{
	printf("Device Not Avaliable\n");
}
//8
void Double_Fault()
{
	printf("Double Fault\n");
}
//9
void Coprocessor_Segment()
{
	printf("Coprocessor Segment\n");
}
//10
void Invalid_TSS()
{
	printf("Invalid TSS\n");
}
//11
void Segment_Not_Present()
{
	printf("Segment_Not_Present\n");
}
//12
void Stack_Fault()
{
	printf("Stack Fault\n");
}
//13
void General_Protection()
{
	printf("General Protection\n");
}
//14
void Page_Fault()
{
	printf("Page Fault\n");
}
//15


void set_gate(int gate, unsigned type, void* addr, unsigned dpl, unsigned seg){
	idt[gate].present = 1;
	idt[gate].dpl = dpl;
	idt[gate].seg_selector = seg;
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
		set_system_gate(i, exception_handler[i]);
	}
	// // idt_desc_t str[num_vec];	
	// int i;
	// for (i=0;i<num_vec;i++) {
	// 	idt[i].present = 1; 
	// 	idt[i].dpl = 0;	
	// 	idt[i].reserved0 = 0;	
	// 	idt[i].size = 1;	
	// 	idt[i].reserved1 = 1;	
	// 	idt[i].reserved2 = 1;	
	// 	idt[i].reserved3 = 1;
	// 	idt[i].reserved4 = 0;
	// 	idt[i].seg_selector = KERNEL_CS;
	// 	SET_IDT_ENTRY(idt[i],exception_handler[i]);
	// }

}






