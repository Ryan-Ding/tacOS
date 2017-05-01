#ifndef PIT_H
#define PIT_H

#include "lib.h"
#include "types.h"
#include "isr_wrappers.h"
#include "scheduler.h"


#define PIT_SCALE 1193180
#define PIT_CHANNEL0 0x40
#define PIT_MODE 0x43
#define PIT_SET 0x36
#define INPUT_FREQ 100
#define MASK_LOWER 0xFF


void pit_init();

void pit_interrupt();


#endif
