#include "idt.h"
#include "x86_desc.h"

void idt_init()
{
	


	
	str.present = 1; \
	str.dpl = 0;	\
	str.reserved0 = 0;	\
	str.size = 1;	\
	str.reserved1 = 1;	\
	str.reserved2 = 1;	\
	str.reserved3 = 0;	\
}