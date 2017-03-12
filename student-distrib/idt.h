#ifndef _IDT_H
#define _IDT_H


#define GATE_INTERRUPT 0
#define GATE_TRAP 1
#define GATE_SYSTEM 2

#define NUM_EXCEPTION 16

extern void idt_init();



extern void Divde_Error_EXCEPTION();
extern void Debug_Exception ();
extern void NMI_Interupt();
extern void Breakpoint_Exception();
extern void Overflow_Exception();
extern void BOUND();
extern void Invalid_Opcode();
extern void Device_Not_Available();
extern void Double_Fault();
extern void Coprocessor_Segment();
extern void Invalid_TSS();
extern void Segment_Not_Present();
extern void Stack_Fault();
extern void General_Protection();
extern void Page_Fault();
//15

void set_gate(int gate, unsigned type, void* addr, unsigned dpl, unsigned seg);
void set_intr_gate(unsigned int n, void* addr);
void set_system_gate(unsigned int n, void* addr);
// void set_system_intr_gate(unsigned int n, void* addr);
void set_trap_gate(unsigned int n, void* addr);
// void set_task_gate(unsigned int n, void* addr);

void (* exception_handler[NUM_EXCEPTION]) = {
	Divde_Error_EXCEPTION,
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


#endif