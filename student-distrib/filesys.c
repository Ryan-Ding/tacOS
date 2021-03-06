#include "filesys.h"

static boot_block_t* boot_block_ptr = NULL;

static uint8_t data_block_flag[AVAILABLE_DATA_BLOCKS];
static uint8_t inode_flag[MAS_DIR_ENTRY_SIZE];

// file operations table
file_ops_table_t rtc_ops_table;
file_ops_table_t dir_ops_table;
file_ops_table_t reg_ops_table;
file_ops_table_t terminal_ops_table;

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
    //uint32_t filesys_end_addr = module_ptr->mod_end;
    //printf("filesys size is: %d \n", (filesys_end_addr - filesys_start_addr) / FILE_SYS_BLOCK_SIZE );
    boot_block_ptr = (boot_block_t*)filesys_start_addr;
    //printf("inode number is: %d \n", boot_block_ptr->num_inodes );
    //printf("data block number is: %d \n", boot_block_ptr->num_data_blocks );
    //printf("matched? %d, \n", (boot_block_ptr->num_inodes + boot_block_ptr->num_data_blocks + 1 ) == (filesys_end_addr - filesys_start_addr) / FILE_SYS_BLOCK_SIZE );

}



/*
init_file_system()
    Description: Initial the filesys
    Input:None
    Output:None
    Return value:None
    Side Effect: ops_tables are filled
*/
void init_file_system()
{
    int i;
  // fill operation tables
    rtc_ops_table.open = rtc_open_syscall;
    rtc_ops_table.read = rtc_read;
    rtc_ops_table.write = rtc_write_syscall;
    rtc_ops_table.close = rtc_close;

    dir_ops_table.open = dir_open;
    dir_ops_table.read = dir_read;
    dir_ops_table.write = dir_write;
    dir_ops_table.close = dir_close;

    reg_ops_table.open = reg_open;
    reg_ops_table.read = reg_read;
    reg_ops_table.write = reg_write;
    reg_ops_table.close = reg_close;

    terminal_ops_table.open = terminal_open;
    terminal_ops_table.read = terminal_read;
    terminal_ops_table.write = terminal_write;
    terminal_ops_table.close = terminal_close;

    for(i=0; i<AVAILABLE_DATA_BLOCKS; i++)
    {
        data_block_flag[i]=0;
    }

        for(i=0; i<MAS_DIR_ENTRY_SIZE; i++)
    {
        inode_flag[i]=0;
    }


}

/*
init_new_fdt()
    Description: Initialize a new fdt
    Input:None
    Output:None
    Return value:None
    Side Effect:  fdt table is filled , mark stdin ,stdout as always opened
*/
void init_new_fdt() {
  // initialize new fdt and fill in the flags ,
    int i;
    if (curr_process == NULL) { return; }
    for (i=0;i<FDT_SIZE;i++)
    {
        curr_process->file_desc_table[i].flag = 0;
    }
    curr_process->file_desc_table[STDIN].flag = 1;
    curr_process->file_desc_table[STDIN].file_ops_table_ptr = &terminal_ops_table;
    curr_process->file_desc_table[STDO].flag = 1;
    curr_process->file_desc_table[STDO].file_ops_table_ptr = &terminal_ops_table;
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
  //edge case check
     if (index >= MAX_DIR_ENTRY_SIZE)
         return -1;
     if (dentry == NULL)
        return -1;
    dentry_t current = boot_block_ptr->dir_entries[index];

    memcpy(dentry,&current,sizeof(dentry_t));
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
        memcpy(buf, ((uint8_t*)data_ptr + offset_temp), bytes_to_be_copied);
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
find_available_data_block()
Description: find available data block
Input:  none
Output: none
Return value: found data block
Side Effect: None
*/

int32_t find_available_data_block(){
    int i,j;

    for(i=0; i<MAX_DIR_ENTRY_SIZE; i++)
    {
        //get the inode block pointer
        inode_block_t * inode_ptr = (inode_block_t *) LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, i);
        uint32_t inode_length = (uint32_t) inode_ptr->length;
        //skip this round if empty
        if (inode_length==0) continue;
        for(j=0; j<inode_length/FILE_SYS_BLOCK_SIZE + 1; j++)
        {
            int32_t inode_data_block_num = inode_ptr->data_blocks[j];
            //set flag as visited
            data_block_flag[inode_data_block_num]=1;
        }
    }

    for(i=0; i<AVAILABLE_DATA_BLOCKS; i++)
    {
        if(data_block_flag[i]==0)
        {
            //set flag as visited
            data_block_flag[i]=1;
            return i;
        }
    }
    return -1;
}

/*
find_available_inode()
Description: find available inode
Input:  none
Output: none
Return value: found inode
Side Effect: None
*/

int32_t find_available_inode(){
    int i;

    //loop though all the inodes in directory entry
    for(i=1; i<MAS_DIR_ENTRY_SIZE; i++)
    {
        if(&(boot_block_ptr->dir_entries[i])==NULL) return -1;
        uint8_t * file_name = boot_block_ptr->dir_entries[i].filename;

        //get the inode block
        inode_block_t * temp_inode_block =(inode_block_t *)LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, boot_block_ptr->dir_entries[i].inode_num);
         uint32_t inode_length = temp_inode_block->length;
        if(strlen((int8_t*)file_name)!=0 && inode_length!=0)
        {
            //set flag as visited
            inode_flag[boot_block_ptr->dir_entries[i].inode_num]=1;
        }
    }

    for(i=1; i<MAS_DIR_ENTRY_SIZE; i++)
    {

        if(inode_flag[i]==0)
        {
            //set flag as visited
            inode_flag[i]=1;
            return i;
        }
    }

    return -1;
}

/*
reg_write
Description: write a regular file
Input:  fd - file descriptor
        buf - the buffer to be used
        nbytes - number of bytes to be written
Output: A buffer contains the file content
Return value: Number of bytes that have been successfully read
Side Effect: None

*/


int32_t reg_write(int32_t fd, const void* buf, int32_t nbytes){
    int i=0;
    if(fd<0 || fd>8) return -1;

    uint32_t copied_bytes;
    uint32_t total_inodes=boot_block_ptr->num_inodes;
    int32_t * data_ptr;

    //if(curr_process->file_desc_table[fd].flag ==0) return 0;

    //dir_write(3, (void*)"qihao", 5);
    uint32_t inode_num = curr_process->file_desc_table[fd].inode;
    //if(!inode_num) return -1;

    inode_block_t * temp_inode_block = (inode_block_t *)LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode_num);
    temp_inode_block->length = nbytes;
    while(nbytes>0){
        int32_t data_block_num = find_available_data_block();
        if(data_block_num==-1) return -1;

        //get the safest number of bytes
        copied_bytes = min(nbytes, FILE_SYS_BLOCK_SIZE);
        copied_bytes = min(copied_bytes, strlen(buf));

        //copy data to the data block
        temp_inode_block->data_blocks[i]=data_block_num;
        data_ptr = (int32_t *) LOCATE_DATA_BLOCK((uint32_t)boot_block_ptr,total_inodes,data_block_num);
        memcpy((uint8_t*)data_ptr, (uint8_t *)buf, copied_bytes);

        nbytes-=FILE_SYS_BLOCK_SIZE;
        i++;        
    }
    return 0;
}

/*
reg_read
Description: write a directory file
Input:  fd - file descriptor
        buf - the buffer to be used
        nbytes - number of bytes to be written
Output: A buffer contains the file content
Return value: Number of bytes that have been successfully read
Side Effect: None

*/



int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){
     if(fd<0 || fd>8) return -1;

     if(buf==0) return -1;

     int32_t dentry_num=boot_block_ptr->num_dir_entries;
     int8_t copied_bytes = min(nbytes, 32);
     copied_bytes = min(copied_bytes, strlen(buf));

     //dentry_t search_for_dir_entry;
     //read_dentry_by_name((uint8_t*)buf, &search_for_dir_entry);
     int32_t inode_num = find_available_inode();

     //write file name to directory entry
     memcpy((uint8_t*)(boot_block_ptr->dir_entries[dentry_num].filename), (int8_t*)buf, copied_bytes);
     boot_block_ptr->dir_entries[dentry_num].filetype=2;
     boot_block_ptr->dir_entries[dentry_num].inode_num=inode_num;
     dentry_num++;
     *((int32_t *)boot_block_ptr)=dentry_num;

     //store pcb information
     curr_process = (pcb_t*) (kernel_stack_top - KERNEL_STACK_ENTRY_SIZE);
     curr_process->file_desc_table[fd].inode=inode_num;
     return 0;
}

/*
fs_open
Description:find the diretory entry corresponding to the named le, allo
ate an unused le desriptor, and set up any data neessary to handle the given type of file;
Input: pointer to file name
Output: N/A
Return value: unused file descriptor; -1 for failure
Side Effect: corresponding file is open

*/


int32_t fs_open(uint8_t* file_name){

    int32_t fd;
    int i;

// null pointer
    if(!file_name)
        return -1;

    dentry_t search_for_dir_entry;
    //printf("The size of inode is: %d\n",sizeof(dentry_t));
    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1){
        return -1;
    }

// find file
    for (i = 0; i<FDT_SIZE + 1; i++)
    {
        if (i == FDT_SIZE)
        {
            //we add FDT_SIZE by 1 because we want to detect the case when all fd entries have been used
            return -1;
        }

        if (curr_process->file_desc_table[i].flag == 0)
        {
            break;
        }

    }

    fd = i;
    // printf("File type: %d ",search_for_dir_entry.filetype );
    uint32_t inode = search_for_dir_entry.inode_num;

    // initialize file descriptor entries
    curr_process->file_desc_table[fd].inode = inode;
    curr_process->file_desc_table[fd].flag = 1;
    curr_process->file_desc_table[fd].file_position = 0;

    // printf("File size: %d      ",length );
    //printf("File name: %s\n", search_for_dir_entry.filename);

    // attach appropriate operation table based on file type
    switch (search_for_dir_entry.filetype) {
        case FILE_TYPE_RTC:
            curr_process->file_desc_table[fd].file_ops_table_ptr = &rtc_ops_table;
            break;
        case FILE_TYPE_DIRECTORY:
            curr_process->file_desc_table[fd].file_ops_table_ptr = &dir_ops_table;
            break;
        case FILE_TYPE_REGULAR:
            curr_process->file_desc_table[fd].file_ops_table_ptr = &reg_ops_table;
            break;
    }

    return fd;
}



/*
reg_open
Description:open a regular file
Input:  filename - the filename to be opened
Output: none
Return value: A file descriptor
Side Effect: corresponding file is open

*/

int32_t reg_open(const uint8_t* filename)
{

    return fs_open((uint8_t*)filename);
}

/*
reg_write
Description: write a regular file
Input:  fd - file descriptor
        buf - the buffer to be used
        nbytes - number of bytes to be written
Output: none
Return value: -1
Side Effect: none

*/

// int32_t reg_write(int32_t fd, const void* buf, int32_t nbytes)
// {
//     return -1;
// }


/*
reg_read
Description: write a regular file
Input:  fd - file descriptor
        buf - the buffer to be used
        nbytes - number of bytes to be written
Output: A buffer contains the file content
Return value: Number of bytes that have been successfully read
Side Effect: None

*/

int32_t reg_read(int32_t fd, void* buf, int32_t nbytes)
{
    if (fd == -1)
        return -1;

    if (curr_process->file_desc_table[fd].flag == 0)
        return 0;

    file_desc_entry_t* fdd =(file_desc_entry_t*)(&(curr_process->file_desc_table[fd])); // get the file directory
    uint32_t inode = fdd->inode;
    uint32_t offset = fdd->file_position;

    int n = read_data(inode,offset,(uint8_t*)buf,nbytes);
    if (n < 0)
        return n;

    fdd->file_position += n;
    return n;
}

/*
reg_close
Description: close a regular file
Input:  fd - file descriptor
Output: none
Return value: 0
Side Effect: None
*/

int32_t reg_close(int32_t fd)
{
    curr_process->file_desc_table[fd].flag = 0;
    return 0;
}

/*
dir_open
Description:open a directory
Input:filename - the filename to be opened
Output: none
Return value: A file descriptor
Side Effect: corresponding file is open

*/

int32_t dir_open(const uint8_t* filename)
{
    return fs_open((uint8_t*)filename);
}

/*
dir_write
Description: write a directory
Input:  fd - file descriptor
        buf - the buffer to be used
        nbytes - number of bytes to be written
Output: none
Return value: 0
Side Effect: none
*/
// int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
// {
//     return -1;
// }

/*
dir_close
Description: close a dir file
Input:  fd - file descriptor
Output: none
Return value: 0
Side Effect: None
*/
int32_t dir_close(int32_t fd)
{
    curr_process->file_desc_table[fd].flag = 0;
    return 0;
}





/*
open
Description:Read the directory
Input: None
Output: None
Return value: number of bytes copied
Side Effect: The info of file inside the directory is printed

*/



int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{

    file_desc_entry_t* fde = (file_desc_entry_t*)(&(curr_process->file_desc_table[fd]));
    int position = fde->file_position;
    uint8_t* name_ptr = boot_block_ptr->dir_entries[position].filename;
    uint8_t* buf_ptr = (uint8_t*)buf;
    int i;
    int n = 0;

    if (name_ptr[0] == '\0')
        return 0;


    for (i = 0; (i< nbytes) &&(i< FILENAME_SIZE); i++)
    {


        *buf_ptr = *name_ptr;
        buf_ptr++;
        name_ptr++;
        n++;
    }

    fde->file_position++;


    return n;
}


/*
get_boot_block_info
Description:get the pointer to the boot block
Input: NONE
Output: None
Return value: the pointer to boot block
Side Effect: NONE
*/

boot_block_t* get_boot_block_info()
{
    return boot_block_ptr;
}



/*
test_read_file_by_index
Description:Test read_dentry_by_index and read_data
Input: index - the index to be read
Output: None
Return value: None
Side Effect: corresponding file is read
*/
void test_read_file_by_index(uint32_t index){
  // NOT USED ANYMORE
//     int j;
//     dentry_t search_for_dir_entry;
//     //printf("The size of inode is: %d\n",sizeof(dentry_t));
//
//     if(read_dentry_by_index(index, &search_for_dir_entry) == -1){
//         return;
//     }
//
// uint32_t inode = search_for_dir_entry.inode_num;
// inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
// uint32_t length = inode_ptr->length;
//
// //printf("Length %d \n", length);
// uint8_t buf[length];
//
// int n = read_data(inode,0,buf,length);
// //int i ;
// // for (i = 0; i< n; i++)
// //     {
// //         printf("%c",buf[i]);
// //     }
// terminal_write(0,buf,n);
//
//
// //printf("filename: %s", search_for_dir_entry.filename);
//
// for (j=0 ; j<FILENAME_SIZE ; j++)
// {
//     if (search_for_dir_entry.filename[j] == '\0')
//         break;
//
//     printf("%c",search_for_dir_entry.filename[j]);

//}

}

/*
test_read_file_by_name
Description:Test read_dentry_by_name and read_data
Input: None
Output: None
Return value: None
Side Effect: corresponding file is read
*/

void test_read_file_by_name()
{

  // NOT USED ANYMORE
    // int j;
    // dentry_t search_for_dir_entry;
    //
    // uint8_t file_name[FILENAME_SIZE] = "frame1.txt";
    //
    // if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1) {return;}
    // uint32_t inode = search_for_dir_entry.inode_num;
    // inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    // uint32_t length = inode_ptr->length;
    // uint8_t buf[length];
    // int n = read_data(inode,2,buf,length);
    // int i ;
    // for (i = 0; i< n; i++)
    // {
    //     printf("(%c,%d)",buf[i],i);
    // }
    // //printf("filename: %s", search_for_dir_entry.filename);
    // for (j=0 ; j<FILENAME_SIZE ; j++)
    // {
    //     if (search_for_dir_entry.filename[j] == '\0')
    //         break;
    //
    //     printf("%c",search_for_dir_entry.filename[j]);
    // }

}


/*
test_dir_read
Description:Test dir_read
Input: None
Output: None
Return value: None
Side Effect: The dentry value is printed
*/

void test_dir_read(){


    dir_read(0,0,0);

}

/*
test_reg_read
Description:Test reg_read
Input: None
Output: None
Return value: None
Side Effect: The file is read
*/

void test_reg_read()
{
  // NOT USED ANYMORE
    int i;
    // dentry_t search_for_dir_entry;
    uint8_t file_name[FILENAME_SIZE] = "frame0.txt";
    // if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1) {return;}
    int32_t fd = reg_open(file_name);
    
    uint32_t inode = curr_process->file_desc_table[fd].inode;
    inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    uint32_t length = inode_ptr->length;
    uint8_t buf[length];
    
    int n = reg_read(fd,buf,length);
    for (i = 0; i< n; i++)
    {
        printf("%c",buf[i]);
    }
    
    printf("filename: %s", file_name);

}
