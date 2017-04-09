#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "filesys.h"
#include "paging.h"

#define PROGRAM_IMAGE_ADDR 0x08048000

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

void init_sys_call();
void program_loader(const uint8_t* filename);


#endif
