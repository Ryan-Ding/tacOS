#ifndef PCB_H
#define PCB_H

#include "types.h"

#define PID_PD_OFFSET 0 // TODO needs verification
#define MAX_PROCESS_NUM 9
#define PROGRAM_IMAGE_PAGE_OFFSET 0x20
#define FIRST_PROGRAM_IMAGE_ADDRESS (0x8 << 20) // 8MB
#define KERNEL_END_ADDR FIRST_PROGRAM_IMAGE_ADDRESS
#define PORGRAM_IMAGE_SIZE (0x4 << 20) // 4MB
#define PROGRAM_IMAGE_START_ADDRESS(pid) ( (1 + pid) << 22 )
#define KERNEL_STACK_ENTRY_SIZE (0x8 << 10)

#define FDT_SIZE 8
#define STDIN 0
#define STDO 1


#define PCB_START_ADDR(pid) (KERNEL_END_ADDR - (pid * KERNEL_STACK_ENTRY_SIZE))

extern uint32_t process_bitmap;

// struct for file operation tables  r/w open close
typedef struct {
   int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
   int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
   int32_t (*open)(const uint8_t* filename);
   int32_t (*close)(int32_t fd);
 } file_ops_table_t;

// struct for file descriptor entry
typedef struct {
  file_ops_table_t* file_ops_table_ptr;
  uint32_t inode;
  uint32_t file_position;
  int32_t flag;
} file_desc_entry_t;

// struct for a pcb
typedef struct pcb_t {
    file_desc_entry_t file_desc_table[FDT_SIZE];
    uint32_t pid;
    uint32_t old_cr3;
    uint32_t old_ebp;
    uint32_t old_esp;
    uint32_t old_esp0;
    uint32_t old_ss;
    uint32_t old_kernel_stack_top;
    struct pcb_t* parent;
} pcb_t;




extern int32_t find_available_pid();

#endif
