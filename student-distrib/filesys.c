#include "filesys.h"


void fetch_boot_block_info (module_t* module_ptr) {
    uint32_t filesys_start_addr = module_ptr->mod_start;
    uint32_t filesys_end_addr = module_ptr->mod_end;
    printf("filesys size is: %d \n", (filesys_end_addr - filesys_start_addr) / FILE_SYS_BLOCK_SIZE );
    boot_block_ptr = (boot_block_t*)filesys_start_addr;
    printf("inode number is: %d \n", boot_block_ptr->num_inodes );
    printf("data block number is: %d \n", boot_block_ptr->num_data_blocks );
    printf("matched? %d, \n", (boot_block_ptr->num_inodes + boot_block_ptr->num_data_blocks + 1 ) == (filesys_end_addr - filesys_start_addr) / FILE_SYS_BLOCK_SIZE );    
}

int32_t read_dentry_by_name (const uint8_t fname, dentry_t* dentry) {
    // TODO
    return -1;
}
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
    // TODO
    return -1;
}
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    // TODO 
    return -1;
}
