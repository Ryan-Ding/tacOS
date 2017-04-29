#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
#include "lib.h"
#include "pcb.h"
#include "terminal.h"
#include "i8259.h"
#include "idt.h"
#include "paging.h"

#define PIT_IRQ 0x00

void switch_task();


#endif
