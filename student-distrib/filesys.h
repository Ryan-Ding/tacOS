#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"
#include "multiboot.h"

#define DENTRY_BYTE_SIZE 64
#define FILENAME_SIZE 32

#define MAX_FILE_NUM_PER_TASK 8

#define FILE_SYS_BLOCK_SIZE (4 * 1024) 

// macros for file types
#define FILE_TYPE_REGULAR 2
#define FILE_TYPE_DIRECTORY 1
#define FILE_TYPE_RTC 0 

typedef struct {
    uint8_t filename[31 + 1]; // null terminator
    uint32_t filetype;
    uint32_t inode_num;
    uint8_t reserved[DENTRY_BYTE_SIZE - FILENAME_SIZE - sizeof(uint32_t) - sizeof(uint32_t)];
} dentry_t;

typedef struct {
    uint32_t num_dir_entries;
    uint32_t num_inodes;
    uint32_t num_data_blocks;
    uint8_t reserved[52];
    dentry_t dir_entries[63];
} boot_block_t;


int32_t read_dentry_by_name (const uint8_t fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

// global variable that keeps track of the boot block information
static boot_block_t* boot_block_ptr = NULL;

extern void fetch_boot_block_info (module_t* module_ptr);



#endif 