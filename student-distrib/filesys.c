#include "filesys.h"
#include "lib.h"

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

/*
read_dentry_by_index
Description: read the dentry value 
Input: index - index of the dir_entries
       dentry - the output dentry
Output: A dentry of the index th dir_entries
Return value: 0 - success
             -1 - failure
Side Effect: None
*/

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {

    if (index >= MAX_DIR_ENTRY_SIZE)
        return -1;

    dentry_t* current = boot_block_ptr->dir_entries;

    strncpy(dentry->filename,current[index].filename,FILENAME_SIZE);
    dentry->filetype = current[index].filetype;
    dentry->inode_num = current[index].inode_num;
    return 0;
}
/*
read_data()
Description: Read the data from the file
Input:  inode - The inode number
        offset - The position of starting point
        buf - the output buffer
        length - the number of bytes to be read
Output: A buffer contains the data
Return value: -1 failure
              otherwise, number of successful read
Side Effect: None
*/

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    // TODO 
    uint32_t 



    return -1;
}
