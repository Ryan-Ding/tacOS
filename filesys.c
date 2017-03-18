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

/*
open
Description:find the diretory entry corresponding to the named le, allo
ate an unused le desriptor, and set up any data neessary to handle the given type of file;
Input: pointer to file name
Output: N/A
Return value: unused file descriptor; -1 for failure
Side Effect: corresponding file is open

*/


int32_t open(char* file_name){

	int32_t fd;

	if(!file_name) 
		return -1;

	dentry_t search_for_dir_entry;

	if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1){
		return -1;
	}
	if(!search_for_dir_entry)
		return -1;

	if(search_for_dir_entry.filetype == FILE_TYPE_RTC)
	{
		rtc_ops_table.open = rtc_open_syscall;
		rtc_ops_table.read = rtc_read;
		rtc_ops_table.write = rtc_write_syscall;
		rtc_ops_table.close = rtc_close;
	}
	else if(search_for_dir_entry.filetype ==FILE_TYPE_DIRECTORY)
	{
		// dir_ops_table.open = dir_open;
		// dir_ops_table.read = dir_read;
		// dir_ops_table.write = dir_write;
		// dir_ops_table.close = dir_close;

	}
	else if(search_for_dir_entry.filetype ==FILE_TYPE_REGULAR)
	{
		// reg_ops_table.open = reg_open;
		// reg_ops_table.read = reg_read;
		// reg_ops_table.write = reg_write;
		// reg_ops_table.close = reg_close;

	}

	return fd;


}