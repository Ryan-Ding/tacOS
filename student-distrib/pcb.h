#ifndef PCB_H
#define PCB_H

#include "types.h"

#define FDT_SIZE 8
#define STDIN 0
#define STDO 1

typedef struct {
   int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
   int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
   int32_t (*open)(const uint8_t* filename);
   int32_t (*close)(int32_t fd);
 } file_ops_table_t;


typedef struct { 
  file_ops_table_t* file_ops_table_ptr; 
  uint32_t inode; 
  uint32_t file_position; 
  int32_t flag; 
} file_desc_entry_t;

typedef struct pcb_t {
    file_desc_entry_t file_desc_table[FDT_SIZE];
    uint32_t pid;
    uint32_t old_ebp;
    uint32_t old_esp;
    uint32_t old_esp0;
    uint32_t current_ebp;
    uint32_t current_esp;
    struct pcb_t* parent; 
} pcb_t;

#endif
