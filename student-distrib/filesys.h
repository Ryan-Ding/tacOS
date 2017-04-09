#ifndef FILESYS_H
#define FILESYS_H

#include "types.h"
#include "multiboot.h"
#include "rtc.h"
#include "sys_call.h"
#include "lib.h"
#include "terminal.h"
#include "sys_call.h"

#define DENTRY_BYTE_SIZE 64
#define FILENAME_SIZE 32

#define MAX_FILE_NUM_PER_TASK 8

#define FILE_SYS_BLOCK_SIZE (4 * 1024)

// macros for file types
#define FILE_TYPE_REGULAR 2
#define FILE_TYPE_DIRECTORY 1
#define FILE_TYPE_RTC 0

#define MAX_DIR_ENTRY_SIZE 63

#define LOCATE_INODE_BLOCK(base,inode) (base + FILE_SYS_BLOCK_SIZE + FILE_SYS_BLOCK_SIZE * inode )
#define LOCATE_DATA_BLOCK(base,inode_all,data_blk) (base + FILE_SYS_BLOCK_SIZE + FILE_SYS_BLOCK_SIZE * inode_all + FILE_SYS_BLOCK_SIZE * data_blk )

#define FDT_SIZE 8
#define STDIN 0
#define STDO 1

typedef struct {
    uint8_t filename[FILENAME_SIZE]; // null terminator
    uint32_t filetype;
    uint32_t inode_num;
    uint8_t reserved[DENTRY_BYTE_SIZE - FILENAME_SIZE - sizeof(uint32_t) - sizeof(uint32_t)];
} dentry_t;

typedef struct {
    uint32_t num_dir_entries;
    uint32_t num_inodes;
    uint32_t num_data_blocks;
    uint8_t reserved[52];
    dentry_t dir_entries[MAX_DIR_ENTRY_SIZE];
} boot_block_t;


typedef struct
{
	uint32_t length;
	uint32_t data_blocks[(FILE_SYS_BLOCK_SIZE - sizeof(uint32_t)) / sizeof(uint32_t)];
} inode_block_t;




int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index ( uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t open(uint8_t* file_name);



void test_dir_read();
void test_reg_read();
void test_read_file_by_index(uint32_t index);
void test_read_file_by_name();


int32_t reg_open(const uint8_t* filename);
int32_t reg_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t reg_read(int32_t fd, void* buf, int32_t nbytes);
int32_t reg_close(int32_t fd);

int32_t dir_open(const uint8_t* filename);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_close(int32_t fd);
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

void init_file_system();

void add_dir_entry(const uint8_t* filename);
void add_inode(const uint8_t* );
boot_block_t* get_boot_block_info();
// global variable that keeps track of the boot block information


extern void fetch_boot_block_info (module_t* module_ptr);



#endif
