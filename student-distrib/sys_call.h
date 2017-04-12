#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "paging.h"
#include "pcb.h"
#include "filesys.h"
#define PROGRAM_IMAGE_ADDR 0x08048000
#define EXECUTABLE_STARTING_ADDR 24
#define THREE_BYTES_SHIFT 24
#define TWO_BYTES_SHIFT 16
#define ONE_BYTE_SHIFT 8
#define MAGIC_NUM_1 0x7f 
#define MAGIC_NUM_2 0x45
#define MAGIC_NUM_3 0x4c
#define MAGIC_NUM_4 0x46

void init_sys_call();
void program_loader(const uint8_t* filename);

extern int32_t system_halt (uint8_t status);
extern int32_t system_execute (const uint8_t* command);
#endif
