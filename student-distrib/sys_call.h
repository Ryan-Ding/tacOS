#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "paging.h"
#include "pcb.h"
#include "filesys.h"
#include "x86_desc.h"

#define PROGRAM_IMAGE_ADDR 0x08048000
#define EXECUTABLE_STARTING_ADDR 24
#define THREE_BYTES_SHIFT 24
#define TWO_BYTES_SHIFT 16
#define ONE_BYTE_SHIFT 8
#define MAGIC_NUM_1 0x7f
#define MAGIC_NUM_2 0x45
#define MAGIC_NUM_3 0x4c
#define MAGIC_NUM_4 0x46
#define _8MB       0x00800000
#define _8KB       0x2000
#define ALIGN_8KB 0xFFFFE000
#define FD_MIN 0
#define FD_MAX 7
#define FD_STDIN 0
#define FD_STDOUT 1

void init_sys_call();
void program_loader(const uint8_t* filename);

extern int32_t system_halt (uint8_t status);
extern int32_t system_execute (const uint8_t* command);
extern int32_t system_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t system_write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t system_open (const uint8_t* filename);
extern int32_t system_close (int32_t fd);

extern pcb_t* curr_process;
extern uint32_t kernel_stack_top;


#endif
