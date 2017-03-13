#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "lib.h"


#define GATE_INTERRUPT 0
#define GATE_TRAP 1
#define GATE_SYSTEM 2

#define IDL0 0
#define IDL3 3

#define NUM_EXCEPTION 20
#define SYSTEM_INT 0x80
#define RESERVE_EXC 15

extern void idt_init();



extern void Divide_Error_Exception();
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
extern void Floating_Point();
extern void Alignment_Check();
extern void Machine_Check_Exception();
extern void SIMD_Floating();

extern void Reserved_Int();

//15

void set_gate(int gate, unsigned type, void* addr, unsigned dpl, unsigned seg);
void set_intr_gate(unsigned int n, void* addr);
void set_system_gate(unsigned int n, void* addr);
// void set_system_intr_gate(unsigned int n, void* addr);
void set_trap_gate(unsigned int n, void* addr);
// void set_task_gate(unsigned int n, void* addr);

void (* exception_handler[NUM_EXCEPTION]);


#endif
