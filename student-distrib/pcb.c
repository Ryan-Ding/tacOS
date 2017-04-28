#include "pcb.h"
uint32_t process_bitmap = 0;
pcb_t* curr_process;
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

/*
 * find_available_pid
 * input: none
 * description: find the pid that is open to use
 * return value: pid number found, or -1 if none is found
 * side effect : process_bitmap is updated as running
 */

int32_t find_available_pid() {

  int8_t idx, bit_mask = 0x1;
  // printf("bitmap: %x \n", process_bitmap);
  for(idx = 0; idx < BITMAP_LENGTH; ++idx ) {
    if ( ((bit_mask << idx) & process_bitmap) == 0 ) {
      process_bitmap |= (bit_mask << idx);
      return idx;
    }
  }

  if (curr_process->parent == NULL) {
    return MAX_PROCESS_NUM - 2;
  }

  printf("You cannot open more process.\n");

  return -1;
}

int32_t check_available_pid() {
  int8_t idx, bit_mask = 0x1;
  // printf("bitmap: %x \n", process_bitmap);
  for(idx = 0; idx < BITMAP_LENGTH; ++idx ) {
    if ( ((bit_mask << idx) & process_bitmap) == 0 ) {
      return 0;
    }
  }
  printf("You cannot open more process. \n");
  return -1;
}
