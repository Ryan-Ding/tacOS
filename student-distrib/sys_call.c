#include "sys_call.h"

boot_block_t* boot_block_ptr;

void init_sys_call(){
	boot_block_ptr = get_boot_block_info();
}

void program_loader(const uint8_t* filename){
	dentry_t search_for_dir_entry;
	if(read_dentry_by_name(filename, &search_for_dir_entry) == -1) {return;}
	uint32_t inode = search_for_dir_entry.inode_num;
	inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    uint32_t length = inode_ptr->length;
    read_data(inode,0,(uint8_t*)PROGRAM_IMAGE_ADDR,length);

}



