#include "filesys.h"

/*
fetch_boot_block_info
    Description: read the filesys information and stores a copy of ptr
    Input: frequency - the frequency to be set
    Output: NONE
    Return value: NONE
    Side Effect: global variable boot_block_ptr is intialized 
*/
void fetch_boot_block_info (module_t* module_ptr) {
    uint32_t filesys_start_addr = module_ptr->mod_start;
    uint32_t filesys_end_addr = module_ptr->mod_end;
    printf("filesys size is: %d \n", (filesys_end_addr - filesys_start_addr) / FILE_SYS_BLOCK_SIZE );
    boot_block_ptr = (boot_block_t*)filesys_start_addr;
    printf("inode number is: %d \n", boot_block_ptr->num_inodes );
    printf("data block number is: %d \n", boot_block_ptr->num_data_blocks );
    printf("matched? %d, \n", (boot_block_ptr->num_inodes + boot_block_ptr->num_data_blocks + 1 ) == (filesys_end_addr - filesys_start_addr) / FILE_SYS_BLOCK_SIZE );    
}


/*
read_dentry_by_name
    Description: read the directory entry given its name
    Input: fname - the name char array to be found
           dentry - directory entry to be initialized
    Output: NONE
    Return value: 0 on success
                  1 on failure
    Side Effect: dentry struct is intialized with appropriate entry values
*/
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    int i; // loop var
    // three cases for failure: 
    // (1) fname is null, 
    // (2) filename not found 
    // (3) dentry is null
    if (fname == NULL || dentry == NULL) { return -1;}
    for (i = 0; i < MAX_DIR_ENTRY_SIZE; ++i) {
        if ( strncmp( (int8_t*) fname, (int8_t*)boot_block_ptr->dir_entries[i].filename, FILENAME_SIZE) == 0 ) {
            // that's when we find that entry with the appropriate name
            // do memberwise copy
            strncpy((int8_t*)dentry->filename, (int8_t*)fname, FILENAME_SIZE);
            dentry->filetype = boot_block_ptr->dir_entries[i].filetype;
            dentry->inode_num = boot_block_ptr->dir_entries[i].inode_num;
            return 0;
        }
    }
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

    strncpy((int8_t*) dentry->filename,(int8_t*) current[index].filename,FILENAME_SIZE);
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
    // uint32_t 



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


int32_t open(uint8_t* file_name){

    int32_t fd;

    if(!file_name) 
        return -1;

    dentry_t search_for_dir_entry;

    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1){
        return -1;
    }
    printf("%s ",search_for_dir_entry.filename );

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

void testing_open_func(){
    int i;
    for (i = 0; i < MAX_DIR_ENTRY_SIZE; ++i) {
        // if(boot_block_ptr->dir_entries[i].filetype != 1 | 2 | 0)
        //     return;

        open((int8_t*)boot_block_ptr->dir_entries[i].filename);
        printf("%s\n", (int8_t*)boot_block_ptr->dir_entries[i].filename);
    }
}