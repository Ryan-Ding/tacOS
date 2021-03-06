#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "paging.h"
#include "pcb.h"
#include "filesys.h"
#include "terminal.h"
#include "x86_desc.h"

#define PROGRAM_IMAGE_ADDR 0x08048000
#define EXECUTABLE_STARTING_ADDR 24
#define THREE_BYTES_SHIFT 24
#define TWO_BYTES_SHIFT 16
#define ONE_BYTE_SHIFT 8
#define CHECK_ELF_1 0x7f
#define CHECK_ELF_2 0x45
#define CHECK_ELF_3 0x4c
#define CHECK_ELF_4 0x46
#define _8MB       0x00800000
#define _8KB       0x2000
#define ALIGN_8KB 0xFFFFE000
#define FD_MIN 0
#define FD_MAX 7
#define FD_STDIN 0
#define FD_STDOUT 1
#define IN_LENGTH 5
#define OUT_LENGTH 6
#define FILE_NAME_BUFFER_SIZE 128
#define ARG_BUFFER_SIZE ARG_SIZE
#define IRET_ESP 0x08400000-4
#define USER_VIDEO_MEM_PAGE_DIRECTORY_OFFSET (PROGRAM_IMAGE_PAGE_OFFSET + 1) // TODO
#define USER_VIDEO_MEM_PAGE_TABLE_OFFSET 0x1
#define LENGTH 4 // 4 BYTES
#define LENGTH_HALF 1
#define VIDEO_MEM_USER_ADDR (PORGRAM_IMAGE_SIZE * USER_VIDEO_MEM_PAGE_DIRECTORY_OFFSET) + (0x1000 * USER_VIDEO_MEM_PAGE_TABLE_OFFSET  )
#define VIDEO_MEM_PHYS_ADDR 0x000B8000
#define PROGRAM_IMG_BASE_ADDR_MASK 0xFF000000

void init_sys_call();
void program_loader(const uint8_t* filename);

extern int32_t system_halt (uint8_t status);
extern int32_t system_execute (const uint8_t* command);
extern int32_t system_read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t system_write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t system_open (const uint8_t* filename);
extern int32_t system_close (int32_t fd);
extern int32_t system_getargs (uint8_t* buf, int32_t nbytes);
extern int32_t system_vidmap (uint8_t** screen_start);

extern void remap_video(uint32_t new_terminal_id);


extern uint32_t kernel_stack_top;


#endif
