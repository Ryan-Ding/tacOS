#include "pcb.h"
/*
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
    //uint32_t odd_cr3;
    uint32_t old_ebp;
    uint32_t old_esp;
    uint32_t old_esp0;
    uint32_t current_ebp;
    uint32_t current_esp;
    struct pcb_t* parent;
} pcb_t;
*/
int32_t find_available_pid() {
  int32_t idx, bit_mask = 0x1, bitmap_length = sizeof(typeof(process_bitmap)) * 8;
  // printf("bitmap: %x \n", process_bitmap);
  for(idx = 0; idx < bitmap_length; ++idx ) {
    if ( ((bit_mask << idx) & process_bitmap) == 0 ) {
      process_bitmap |= (bit_mask << idx);
      return idx;
    }
  }
  return -1;
}
