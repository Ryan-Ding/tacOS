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
    if (fname == NULL || dentry == NULL) { return -1; }
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
    uint32_t bytes_to_be_copied = 0;
    uint32_t count = 0; // number of bytes successfully stored in the buffer
    uint32_t index = offset / FILE_SYS_BLOCK_SIZE; // the data block index to be started with
    uint32_t start_block_offset =  offset % FILE_SYS_BLOCK_SIZE; // the starting position within the first data block
    uint32_t offset_temp = start_block_offset;
    uint32_t* data_ptr = NULL ; // pointer to the current data block to be copied
    inode_block_t* inode_ptr = NULL; // inode pointer
    uint32_t num_data = boot_block_ptr->num_data_blocks;
    uint32_t num_inodes = boot_block_ptr->num_inodes; // total number of inode blocks, defined for convenience

    if (inode >= num_inodes) { return -1; }

    inode_ptr = (inode_block_t*) LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode); 
    

    // if the length is larger than what is actually remained, we clip it 
    if ((length + offset) > (inode_ptr->length))
    {
        length = inode_ptr->length - offset;
    }

    if (offset >= inode_ptr->length) { return 0; } // if offset is not valid, we copy nothing

    while (length > 0) {
        // if the data block value is not valid, return what we have copied so far
        if (index > num_data) { return count; } 

        data_ptr = (uint32_t*) LOCATE_DATA_BLOCK((uint32_t)boot_block_ptr, num_inodes, (inode_ptr->data_blocks)[index] );
        bytes_to_be_copied =  min(length, FILE_SYS_BLOCK_SIZE - offset_temp);
        memcpy(buf, data_ptr + offset_temp, bytes_to_be_copied);
        // prepare for further copies
        length -= bytes_to_be_copied;
        count += bytes_to_be_copied;
        buf += bytes_to_be_copied;  // update buf ptr position to always point to the first empty space 
        if (bytes_to_be_copied == (FILE_SYS_BLOCK_SIZE - offset_temp)) {
            ++index; // the current data block has all been copied, start a new block next time
            offset_temp = 0;
        }  else {
            offset_temp += bytes_to_be_copied; // there are still leftovers in the same data block
        }
    }

    return count;
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
    //printf("The size of inode is: %d\n",sizeof(dentry_t));
    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1){
        return -1;
    }

    printf("File type: %d ",search_for_dir_entry.filetype );
    uint32_t inode = search_for_dir_entry.inode_num;
    inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    uint32_t length = inode_ptr->length;

    printf("File size: %d      ",length );
    printf("File name: %s\n", search_for_dir_entry.filename);

    switch (search_for_dir_entry.filetype) {
        case FILE_TYPE_RTC:
            rtc_ops_table.open = rtc_open_syscall;
            rtc_ops_table.read = rtc_read;
            rtc_ops_table.write = rtc_write_syscall;
            rtc_ops_table.close = rtc_close;
            break;
        case FILE_TYPE_DIRECTORY:
            // dir_ops_table.open = dir_open;
            // dir_ops_table.read = dir_read;
            // dir_ops_table.write = dir_write;
            // dir_ops_table.close = dir_close;
            break;
        case FILE_TYPE_REGULAR:
            // reg_ops_table.open = reg_open;
            // reg_ops_table.read = reg_read;
            // reg_ops_table.write = reg_write;
            // reg_ops_table.close = reg_close;
            break;
    }

    return fd;


}



void test_read_file(){
int index = 10;
uint32_t inode = boot_block_ptr->dir_entries[index].inode_num;
inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
uint32_t length = inode_ptr->length;

//printf("Length %d \n", length);
uint8_t buf[length];

int nbytes = read_data(inode,0,buf,length);
int i ;
for (i = 0; i< nbytes; i++)
    {
        printf("%c",buf[i]);
    }
printf("filename: %s", boot_block_ptr->dir_entries[index].filename);

}


void test_read_file_by_name()
{
    dentry_t search_for_dir_entry;
    uint8_t file_name[32] = "frame1.txt"; 
    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1) {return;}
    uint32_t inode = search_for_dir_entry.inode_num;
    inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    uint32_t length = inode_ptr->length;
    uint8_t buf[length];
    int nbytes = read_data(inode,0,buf,length);
    int i ;
    for (i = 0; i< nbytes; i++)
    {
        printf("%c",buf[i]);
    }
    printf("filename: %s", search_for_dir_entry.filename);

}




void testing_open_func(){
    int i;
    for (i = 0; i < MAX_DIR_ENTRY_SIZE; ++i) {
        if(boot_block_ptr->dir_entries[i].filename[0] == 0)
            continue;

        open((uint8_t*)boot_block_ptr->dir_entries[i].filename);
    }
}
