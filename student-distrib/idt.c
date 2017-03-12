#include "idt.h"
#include "x86_desc.h"
#include "lib.h"



//0
void Divde_Error_EXCEPTION()
{
	printf("Divide Error\n");
	while (1)
	{}
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






void idt_init() {
	idt_desc_t str[num_vec];	
	int i;
	for (i=0;i<num_vec;i++) {
		str[i].present = 1; 
		str[i].dpl = 0;	
		str[i].reserved0 = 0;	
		str[i].size = 1;	
		str[i].reserved1 = 1;	
		str[i].reserved2 = 1;
		str[i].reserved3 = 1;
		str[i].reserved4 = 0;
		str[i].seg_selector = KERNEL_CS;
		SET_IDT_ENTRY(str[i],exception_handler[i]);

		idt[i] = str[i];
	}

}






